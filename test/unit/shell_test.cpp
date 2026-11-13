#include <gtest/gtest.h>

#include <support/test_support.h>

namespace myzsh {
namespace {

TEST(Shell, ConstructionIsCheap) {
  Shell shell;
  EXPECT_EQ(shell.status, 0);
  EXPECT_GT(shell.shell_pid, 0);
}

TEST(Shell, InitForTestSetsDefaults) {
  auto shell = test::make_shell();
  EXPECT_TRUE(shell->option(Option::Glob));
  EXPECT_TRUE(shell->builtins.find("print"));
  EXPECT_TRUE(shell->params.is_set("ZSH_VERSION"));
  EXPECT_FALSE(shell->interactive);
}

TEST(Shell, RunStringReturnsStatus) {
  auto shell = test::make_shell();
  EXPECT_EQ(test::run(*shell, "true").status, 0);
  EXPECT_EQ(test::run(*shell, "false").status, 1);
  EXPECT_EQ(shell->status, 1);
}

TEST(Shell, ParseErrorIsReported) {
  test::Captured result = test::run("echo a )");
  EXPECT_EQ(result.status, 1);
  EXPECT_EQ(result.err, "zsh:1: parse error near `)'\n");
}

TEST(Shell, WholeScriptIsParsedBeforeRunning) {
  test::Captured result = test::run("print first\nx=(; print never");
  EXPECT_EQ(result.out, "");
  EXPECT_EQ(result.status, 1);
}

TEST(Shell, ShellErrorAbortsScript) {
  test::Captured result = test::run("print a; print ${x:?gone}; print b");
  EXPECT_EQ(result.out, "a\n");
  EXPECT_EQ(result.err, "zsh:1: x: gone\n");
  EXPECT_EQ(result.status, 1);
}

TEST(Shell, ErrorNameAndLine) {
  auto shell = test::make_shell();
  shell->error_name = "script.zsh";
  test::Captured result = test::run(*shell, "\n\nnosuch_cmd_xyz");
  EXPECT_EQ(result.err, "script.zsh:3: command not found: nosuch_cmd_xyz\n");
}

TEST(Shell, ReportBuiltinError) {
  auto shell = test::make_shell();
  test::Captured result = test::capture([&] {
    shell->report_builtin_error("cd", "no such file or directory: /x");
    return 0;
  });
  EXPECT_EQ(result.err, "zsh:cd:1: no such file or directory: /x\n");
}

TEST(Shell, RunFileWithArguments) {
  test::TempDir dir;
  sf::path script = dir.write("s.zsh", "print $# $1\nreturn 3\nprint no\n");
  auto shell = test::make_shell();
  vec<str> args{"a", "b"};
  test::Captured result = test::capture([&] {
    return shell->run_file(script.string(), &args);
  });
  EXPECT_EQ(result.out, "2 a\n");
  EXPECT_EQ(result.status, 3);
}

TEST(Shell, RunFileMissing) {
  auto shell = test::make_shell();
  test::Captured result = test::capture([&] {
    return shell->run_file("/nonexistent/file.zsh");
  });
  EXPECT_EQ(result.status, 127);
  EXPECT_FALSE(result.err.empty());
}

TEST(Shell, StatePersistsAcrossRunString) {
  auto shell = test::make_shell();
  test::run(*shell, "x=1; f() { print f$x }");
  EXPECT_EQ(test::run(*shell, "f").out, "f1\n");
}

TEST(Shell, ExitTrapRunsOnRunExit) {
  auto shell = test::make_shell();
  test::Captured result = test::run(*shell, "trap 'print bye' EXIT; exit 2");
  EXPECT_EQ(result.out, "bye\n");
  EXPECT_EQ(result.status, 2);
  EXPECT_TRUE(shell->flow.exiting);
}

TEST(Shell, InitFromInvocation) {
  auto invocation = parse_command_line({"zsh", "-f", "-o", "extendedglob",
                                        "-c", "print $0 $1", "name", "arg"});
  ASSERT_TRUE(invocation);
  Shell shell;
  char* envp[] = {nullptr};
  shell.init(*invocation, envp);
  EXPECT_TRUE(shell.option(Option::Extendedglob));
  EXPECT_FALSE(shell.option(Option::Rcs));
  test::Captured result = test::capture([&] { return shell.run(*invocation); });
  EXPECT_EQ(result.out, "name arg\n");
}

}  // namespace
}  // namespace myzsh
