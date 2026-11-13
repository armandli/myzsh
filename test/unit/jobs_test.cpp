#include <gtest/gtest.h>

#include <sys/wait.h>

#include <csignal>

#include <jobs/jobs.h>

namespace myzsh {
namespace {

// waitpid() status encodings, built portably.
int exited(int code) { return (code & 0xff) << 8; }
int signaled(int signal) { return signal & 0x7f; }
int stopped(int signal) { return ((signal & 0xff) << 8) | 0x7f; }

struct JobTableTest : ::testing::Test {
  JobTable table;

  Job& add(pid_t pid, strv text) {
    Job& job = table.add(pid, text, false);
    job.processes.push_back({pid, str(text), ProcessState::Running, 0});
    return job;
  }
};

TEST(WaitStatus, Encodings) {
  // The macOS W* macros need lvalues.
  int exit_status = exited(3);
  int signal_status = signaled(SIGTERM);
  int stop_status = stopped(SIGTSTP);
  EXPECT_TRUE(WIFEXITED(exit_status));
  EXPECT_EQ(WEXITSTATUS(exit_status), 3);
  EXPECT_TRUE(WIFSIGNALED(signal_status));
  EXPECT_TRUE(WIFSTOPPED(stop_status));
}

TEST_F(JobTableTest, NumbersAreLowestFree) {
  EXPECT_EQ(add(100, "a").number, 1);
  EXPECT_EQ(add(200, "b").number, 2);
  EXPECT_EQ(add(300, "c").number, 3);
  EXPECT_TRUE(table.remove(2));
  EXPECT_EQ(add(400, "d").number, 2);
  EXPECT_EQ(table.numbers(), (vec<int>{1, 2, 3}));
}

TEST_F(JobTableTest, FindByNumberAndPid) {
  add(100, "a");
  add(200, "b");
  ASSERT_TRUE(table.find(2));
  EXPECT_EQ(table.find(2)->get().text, "b");
  ASSERT_TRUE(table.find_by_pid(100));
  EXPECT_EQ(table.find_by_pid(100)->get().number, 1);
  EXPECT_FALSE(table.find(9));
  EXPECT_FALSE(table.find_by_pid(999));
}

TEST_F(JobTableTest, CurrentAndPrevious) {
  add(100, "first");
  add(200, "second");
  EXPECT_EQ(table.current(), 2);
  EXPECT_EQ(table.previous(), 1);
  table.make_current(1);
  EXPECT_EQ(table.current(), 1);
  EXPECT_EQ(table.previous(), 2);
  table.remove(1);
  EXPECT_EQ(table.current(), 2);
}

TEST_F(JobTableTest, ResolveSpecs) {
  add(100, "sleep 100");
  add(200, "vim notes.txt");
  add(300, "make all");
  EXPECT_EQ(table.resolve("%1"), 1);
  EXPECT_EQ(table.resolve("%%"), 3);
  EXPECT_EQ(table.resolve("%+"), 3);
  EXPECT_EQ(table.resolve("%"), 3);
  EXPECT_EQ(table.resolve("%-"), 2);
  EXPECT_EQ(table.resolve("%vim"), 2);
  EXPECT_EQ(table.resolve("%?notes"), 2);
  EXPECT_EQ(table.resolve("%9"), s::nullopt);
  EXPECT_EQ(table.resolve("%nothing"), s::nullopt);
}

TEST_F(JobTableTest, UpdateTracksStates) {
  Job& job = add(100, "sleep 10");
  EXPECT_EQ(job.state(), ProcessState::Running);
  EXPECT_EQ(table.update(100, stopped(SIGTSTP)), 1);
  EXPECT_EQ(table.find(1)->get().state(), ProcessState::Stopped);
  EXPECT_EQ(table.update(100, exited(0)), 1);
  EXPECT_EQ(table.find(1)->get().state(), ProcessState::Done);
  EXPECT_EQ(table.update(555, exited(0)), s::nullopt);
}

TEST_F(JobTableTest, JobDoneOnlyWhenAllProcessesDone) {
  Job& job = table.add(100, "a | b", false);
  job.processes.push_back({100, "a", ProcessState::Running, 0});
  job.processes.push_back({101, "b", ProcessState::Running, 0});
  table.update(100, exited(0));
  EXPECT_EQ(table.find(1)->get().state(), ProcessState::Running);
  table.update(101, exited(2));
  EXPECT_EQ(table.find(1)->get().state(), ProcessState::Done);
  EXPECT_EQ(table.find(1)->get().exit_status(), 2);
}

TEST_F(JobTableTest, ExitStatusForSignals) {
  add(100, "x");
  table.update(100, signaled(SIGKILL));
  EXPECT_EQ(table.find(1)->get().exit_status(), 128 + SIGKILL);
}

// Formats from zsh 5.9 (`sleep 1 & jobs`).
TEST_F(JobTableTest, Format) {
  Job& job = add(100, "sleep 1");
  EXPECT_EQ(table.format(job, false), "[1]  + running    sleep 1");
  EXPECT_EQ(table.format(job, true), "[1]  + 100 running    sleep 1");
  Job& other = add(200, "sleep 2");
  table.update(100, stopped(SIGTSTP));
  table.make_current(2);
  EXPECT_EQ(table.format(table.find(1)->get(), false),
            "[1]  - suspended  sleep 1");
  table.update(200, exited(3));
  EXPECT_EQ(table.format(other, false), "[2]  + exit 3     sleep 2");
}

TEST_F(JobTableTest, PruneRemovesNotifiedDoneJobs) {
  add(100, "a");
  add(200, "b");
  table.update(100, exited(0));
  table.find(1)->get().notified = true;
  table.prune();
  EXPECT_EQ(table.numbers(), vec<int>{2});
}

TEST(DescribeWaitStatus, Words) {
  EXPECT_EQ(describe_wait_status(exited(0), ProcessState::Done), "done");
  EXPECT_EQ(describe_wait_status(exited(3), ProcessState::Done), "exit 3");
  EXPECT_EQ(describe_wait_status(signaled(SIGINT), ProcessState::Done),
            "interrupt");
  EXPECT_EQ(describe_wait_status(signaled(SIGTERM), ProcessState::Done),
            "terminated");
  EXPECT_EQ(describe_wait_status(signaled(SIGKILL), ProcessState::Done),
            "killed");
  EXPECT_EQ(describe_wait_status(stopped(SIGTSTP), ProcessState::Stopped),
            "suspended");
  EXPECT_EQ(describe_wait_status(stopped(SIGTTIN), ProcessState::Stopped),
            "suspended (tty input)");
  EXPECT_EQ(describe_wait_status(0, ProcessState::Running), "running");
}

}  // namespace
}  // namespace myzsh
