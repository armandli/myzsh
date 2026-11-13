#include <gtest/gtest.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <exec/redirect.h>
#include <parse/parser.h>
#include <support/test_support.h>
#include <util/error.h>

namespace myzsh {
namespace {

struct RedirectTest : ::testing::Test {
  uptr<Shell> shell = test::make_shell();
  test::TempDir dir;
  opt<test::ScopedCwd> cwd;

  void SetUp() override { cwd.emplace(dir.path()); }

  vec<ast::Redirect> redirects(strv source) {
    auto list = parse(str("cmd ") + str(source));
    EXPECT_TRUE(list) << source;
    const auto& command =
      list->items.at(0).sublist.items.at(0).pipeline.commands.at(0);
    return s::get<ast::SimpleCommand>(command.node).redirects;
  }

  static void write_fd(int fd, strv text) {
    ASSERT_EQ(::write(fd, text.data(), text.size()),
              static_cast<ssize_t>(text.size()));
  }
};

TEST_F(RedirectTest, WriteAndRestore) {
  int saved = dup(1);
  {
    Redirector redirector(*shell, true);
    redirector.apply(redirects("> out.txt"));
    write_fd(1, "hello\n");
    redirector.restore();
  }
  EXPECT_EQ(test::read_file(dir / "out.txt"), "hello\n");
  // fd 1 must be the original again.
  struct stat before{};
  struct stat after{};
  fstat(saved, &before);
  fstat(1, &after);
  EXPECT_EQ(before.st_ino, after.st_ino);
  close(saved);
}

TEST_F(RedirectTest, AppendKeepsContent) {
  dir.write("log", "a\n");
  Redirector redirector(*shell, true);
  redirector.apply(redirects(">> log"));
  write_fd(1, "b\n");
  redirector.restore();
  EXPECT_EQ(test::read_file(dir / "log"), "a\nb\n");
}

TEST_F(RedirectTest, ReadFromFile) {
  dir.write("in", "data");
  Redirector redirector(*shell, true);
  redirector.apply(redirects("< in"));
  char buffer[8] = {};
  EXPECT_EQ(read(0, buffer, sizeof buffer), 4);
  redirector.restore();
  EXPECT_EQ(strv(buffer, 4), "data");
}

TEST_F(RedirectTest, MissingInputFileIsError) {
  Redirector redirector(*shell, true);
  try {
    redirector.apply(redirects("< nonexistent"));
    FAIL();
  } catch (const ShellError& error) {
    EXPECT_STREQ(error.what(), "no such file or directory: nonexistent");
  }
}

TEST_F(RedirectTest, DuplicateAndClose) {
  Redirector redirector(*shell, true);
  redirector.apply(redirects("> both 2>&1"));
  write_fd(2, "err\n");
  write_fd(1, "out\n");
  redirector.restore();
  EXPECT_EQ(test::read_file(dir / "both"), "err\nout\n");

  Redirector closer(*shell, true);
  closer.apply(redirects("3>&-"));
  EXPECT_EQ(fcntl(3, F_GETFD), -1);
  closer.restore();
}

TEST_F(RedirectTest, ErrWriteRedirectsBoth) {
  Redirector redirector(*shell, true);
  redirector.apply(redirects("&> both"));
  write_fd(1, "1");
  write_fd(2, "2");
  redirector.restore();
  EXPECT_EQ(test::read_file(dir / "both"), "12");
}

TEST_F(RedirectTest, NoClobber) {
  dir.write("exists", "keep");
  shell->options.set(Option::Clobber, false);
  Redirector redirector(*shell, true);
  try {
    redirector.apply(redirects("> exists"));
    FAIL();
  } catch (const ShellError& error) {
    EXPECT_STREQ(error.what(), "file exists: exists");
  }
  Redirector forced(*shell, true);
  forced.apply(redirects(">| exists"));
  forced.restore();
  EXPECT_EQ(test::read_file(dir / "exists"), "");
}

TEST_F(RedirectTest, HereString) {
  Redirector redirector(*shell, true);
  redirector.apply(redirects("<<< 'some text'"));
  char buffer[32] = {};
  ssize_t count = read(0, buffer, sizeof buffer);
  redirector.restore();
  EXPECT_EQ(strv(buffer, static_cast<size_t>(count)), "some text\n");
}

TEST_F(RedirectTest, HereDocumentExpands) {
  shell->params.set_scalar("name", "world");
  auto list = parse("cat <<EOF\nhello $name\nEOF\n");
  ASSERT_TRUE(list);
  const auto& command =
    list->items[0].sublist.items[0].pipeline.commands[0];
  Redirector redirector(*shell, true);
  redirector.apply(s::get<ast::SimpleCommand>(command.node).redirects);
  char buffer[32] = {};
  ssize_t count = read(0, buffer, sizeof buffer);
  redirector.restore();
  EXPECT_EQ(strv(buffer, static_cast<size_t>(count)), "hello world\n");
}

TEST_F(RedirectTest, FdVariable) {
  Redirector redirector(*shell, false);
  redirector.apply(redirects("{myfd}> fdvar.txt"));
  auto fd = shell->params.get_integer("myfd");
  ASSERT_TRUE(fd);
  EXPECT_GE(*fd, 10);
  write_fd(static_cast<int>(*fd), "via var");
  close(static_cast<int>(*fd));
  EXPECT_EQ(test::read_file(dir / "fdvar.txt"), "via var");
}

TEST_F(RedirectTest, MultiosTee) {
  Redirector redirector(*shell, true);
  redirector.apply(redirects("> a > b"));
  write_fd(1, "x");
  redirector.restore();
  EXPECT_EQ(test::read_file(dir / "a"), "x");
  EXPECT_EQ(test::read_file(dir / "b"), "x");
}

TEST_F(RedirectTest, DestructorRestores) {
  int saved = dup(1);
  {
    Redirector redirector(*shell, true);
    redirector.apply(redirects("> scoped"));
  }
  write_fd(1, "");
  struct stat before{};
  struct stat after{};
  fstat(saved, &before);
  fstat(1, &after);
  EXPECT_EQ(before.st_ino, after.st_ino);
  close(saved);
}

}  // namespace
}  // namespace myzsh
