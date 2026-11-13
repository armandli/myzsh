#include <gtest/gtest.h>

#include <fcntl.h>
#include <unistd.h>

#include <parse/input.h>
#include <support/test_support.h>

namespace myzsh {
namespace {

str read_rest(InputSource& input) {
  str result;
  for (int c = input.next_char(); c != InputSource::END_OF_INPUT;
       c = input.next_char()) {
    result += static_cast<char>(c);
  }
  return result;
}

str read_through_newline(InputSource& input) {
  str result;
  for (int c = input.next_char(); c != InputSource::END_OF_INPUT;
       c = input.next_char()) {
    result += static_cast<char>(c);
    if (c == '\n') {
      break;
    }
  }
  return result;
}

TEST(InputSource, StringReadsToEnd) {
  StringInput input("ab");
  EXPECT_EQ(input.next_char(), 'a');
  EXPECT_EQ(input.next_char(), 'b');
  EXPECT_EQ(input.next_char(), InputSource::END_OF_INPUT);
  EXPECT_EQ(input.next_char(), InputSource::END_OF_INPUT);
}

TEST(InputSource, UngetAcrossLines) {
  StringInput input("a\nb");
  EXPECT_EQ(input.next_char(), 'a');
  EXPECT_EQ(input.next_char(), '\n');
  input.unget('\n');
  input.unget('a');
  EXPECT_EQ(read_rest(input), "a\nb");
}

TEST(InputSource, PushedTextIsReadFirst) {
  StringInput input("ll -a");
  EXPECT_EQ(input.next_char(), 'l');
  EXPECT_EQ(input.next_char(), 'l');
  input.push_text("ls -l");
  EXPECT_EQ(read_rest(input), "ls -l -a");
}

TEST(InputSource, NestedPushesReadInnermostFirst) {
  StringInput input("!");
  input.push_text("outer");
  EXPECT_EQ(input.next_char(), 'o');
  input.push_text("IN");
  EXPECT_EQ(read_rest(input), "INuter!");
}

// On a pipe nothing past what the caller asked for may be consumed: the
// rest belongs to commands the script runs.
TEST(FdInput, PipeLeavesRestUnread) {
  int fds[2];
  ASSERT_EQ(pipe(fds), 0);
  str data = "echo a\nrest\n";
  ASSERT_EQ(write(fds[1], data.data(), data.size()),
            static_cast<ssize_t>(data.size()));
  close(fds[1]);
  FdInput input(fds[0]);
  EXPECT_FALSE(input.seekable());
  EXPECT_EQ(read_through_newline(input), "echo a\n");
  input.sync_position();
  char buffer[16] = {};
  EXPECT_EQ(read(fds[0], buffer, sizeof buffer), 5);
  EXPECT_EQ(str(buffer, 5), "rest\n");
  close(fds[0]);
}

// On a file the input may read ahead, but sync_position() seeks back to
// the first unconsumed byte.
TEST(FdInput, FileSeeksBackOnSync) {
  test::TempDir dir;
  sf::path file = dir.write("script", "echo a\nrest\n");
  int fd = open(file.c_str(), O_RDONLY);
  ASSERT_GE(fd, 0);
  FdInput input(fd);
  EXPECT_TRUE(input.seekable());
  EXPECT_EQ(read_through_newline(input), "echo a\n");
  input.sync_position();
  EXPECT_EQ(lseek(fd, 0, SEEK_CUR), 7);
  EXPECT_EQ(read_rest(input), "rest\n");
  close(fd);
}

TEST(FdInput, EndOfInput) {
  test::TempDir dir;
  sf::path file = dir.write("empty");
  int fd = open(file.c_str(), O_RDONLY);
  ASSERT_GE(fd, 0);
  FdInput input(fd);
  EXPECT_EQ(input.next_char(), InputSource::END_OF_INPUT);
  close(fd);
}

}  // namespace
}  // namespace myzsh
