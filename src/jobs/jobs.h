#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>

#include <util/aliases.h>

namespace myzsh {

enum class ProcessState : int {
  Running,
  Stopped,
  Done,
};

struct Process {
  pid_t pid = 0;
  str text;
  ProcessState state = ProcessState::Running;
  // Raw waitpid() status once Stopped/Done.
  int wait_status = 0;
};

struct Job {
  int number = 0;
  pid_t pgid = 0;
  str text;
  vec<Process> processes;
  bool foreground = false;
  bool disowned = false;
  // State change already reported to the user.
  bool notified = false;
  // Directory the job was started in, shown by `jobs -d`.
  str directory;

  // Stopped if any process is stopped, Done if all are done.
  ProcessState state() const;
  // Shell-style status of the last process: exit code, or 128+signal.
  int exit_status() const;
};

// Job table (Src/jobs.c jobtab). Process-level operations (fork, wait,
// tcsetpgrp) live in the executor and builtins; this keeps the bookkeeping
// so it can be unit-tested without real processes.
struct JobTable {
  // Lowest free number >= 1.
  Job& add(pid_t pgid, strv text, bool foreground);
  bool remove(int number);
  opt<s::reference_wrapper<Job>> find(int number);
  opt<s::reference_wrapper<Job>> find_by_pid(pid_t pid);

  // %N %% %+ % %- %string (prefix of text) %?string (substring).
  // Returns nullopt for "no such job" or ambiguous specs.
  opt<int> resolve(strv spec) const;
  // Current (+) and previous (-) jobs, as used by fg/bg without args.
  opt<int> current() const;
  opt<int> previous() const;
  // Makes the job current, pushing the old current to previous.
  void make_current(int number);

  // Record a waitpid() result. Returns the job number affected, if any.
  opt<int> update(pid_t pid, int wait_status);

  // jobs output: "[1]  + running    sleep 10"; long adds pids
  // ("[1]  + 1234 running    sleep 10"); state words follow zsh:
  // running, suspended, done, exit N, interrupt, terminated, killed...
  str format(const Job& job, bool long_format) const;
  vec<int> numbers() const;
  // Removes finished jobs that were reported.
  void prune();

private:
  vec<Job> mJobs;
  opt<int> mCurrent;
  opt<int> mPrevious;
};

// "suspended (signal)", "killed", "done", "exit 3"...
str describe_wait_status(int wait_status, ProcessState state);

}  // namespace myzsh

#endif  // JOBS_H
