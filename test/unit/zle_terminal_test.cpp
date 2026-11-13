#include <gtest/gtest.h>

#include <unistd.h>

#include <zle/terminal.h>

namespace myzsh {
namespace {

using enum KeyRead::Status;

struct TerminalTest : ::testing::Test {
  int fds[2] = {-1, -1};

  void SetUp() override { ASSERT_EQ(pipe(fds), 0); }
  void TearDown() override {
    for (int fd : fds) {
      if (fd >= 0) {
        close(fd);
      }
    }
  }
  void close_writer() {
    close(fds[1]);
    fds[1] = -1;
  }
};

TEST_F(TerminalTest, NotATty) {
  Terminal terminal(fds[0]);
  EXPECT_FALSE(terminal.is_tty());
}

TEST_F(TerminalTest, ReadsByteThenEndOfFile) {
  ASSERT_EQ(write(fds[1], "x", 1), 1);
  close_writer();
  Terminal terminal(fds[0]);
  KeyRead first = terminal.read_byte(-1);
  EXPECT_EQ(first.status, Byte);
  EXPECT_EQ(first.byte, 'x');
  EXPECT_EQ(terminal.read_byte(-1).status, EndOfFile);
}

TEST_F(TerminalTest, TimesOutWhenNothingToRead) {
  Terminal terminal(fds[0]);
  EXPECT_EQ(terminal.read_byte(10).status, TimedOut);
}

}  // namespace
}  // namespace myzsh
