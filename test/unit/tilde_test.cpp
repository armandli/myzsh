#include <gtest/gtest.h>

#include <expand/tilde.h>
#include <support/test_support.h>
#include <util/error.h>

namespace myzsh {
namespace {

struct TildeTest : ::testing::Test {
  uptr<Shell> shell = test::make_shell();

  void SetUp() override {
    shell->params.set_scalar("HOME", "/home/me");
    shell->params.set_scalar("PWD", "/work/now");
    shell->params.set_scalar("OLDPWD", "/work/before");
    shell->directory_stack = {"/stack/one", "/stack/two"};
    shell->named_dirs.add("proj", "/src/project");
  }

  str expand(strv word, bool assignment = false) {
    return expand_tilde(QString(word), *shell, assignment).text;
  }
};

TEST_F(TildeTest, Home) {
  EXPECT_EQ(expand("~"), "/home/me");
  EXPECT_EQ(expand("~/x"), "/home/me/x");
}

TEST_F(TildeTest, PwdAndOldPwd) {
  EXPECT_EQ(expand("~+"), "/work/now");
  EXPECT_EQ(expand("~-/f"), "/work/before/f");
}

TEST_F(TildeTest, DirectoryStack) {
  // ~0 is $PWD, ~1 the first stacked directory.
  EXPECT_EQ(expand("~0"), "/work/now");
  EXPECT_EQ(expand("~1"), "/stack/one");
  EXPECT_EQ(expand("~+2"), "/stack/two");
  EXPECT_EQ(expand("~-0"), "/stack/two");
}

TEST_F(TildeTest, NamedDirectory) {
  EXPECT_EQ(expand("~proj/src"), "/src/project/src");
}

TEST_F(TildeTest, UnknownUserIsError) {
  try {
    expand("~no_such_user_xyz");
    FAIL();
  } catch (const ShellError& error) {
    EXPECT_STREQ(error.what(),
                 "no such user or named directory: no_such_user_xyz");
  }
}

TEST_F(TildeTest, NotAtStartIsLiteral) {
  EXPECT_EQ(expand("a~"), "a~");
  EXPECT_EQ(expand("x:~/y"), "x:~/y");
}

TEST_F(TildeTest, AssignmentExpandsAfterColons) {
  EXPECT_EQ(expand("~/a:~/b", true), "/home/me/a:/home/me/b");
}

TEST_F(TildeTest, QuotedTildeIsLiteral) {
  EXPECT_EQ(expand_tilde(QString::literal("~"), *shell, false).text, "~");
}

TEST_F(TildeTest, ResultIsMarkedQuoted) {
  shell->params.set_scalar("HOME", "/odd[dir]");
  QString result = expand_tilde(QString("~/x*"), *shell, false);
  EXPECT_EQ(result.text, "/odd[dir]/x*");
  EXPECT_TRUE(result.is_quoted(4));
  EXPECT_FALSE(result.is_quoted(result.size() - 1));
}

TEST_F(TildeTest, EqualsCommand) {
  shell->params.set_scalar("PATH", "/bin:/usr/bin");
  EXPECT_EQ(expand("=sh"), "/bin/sh");
  EXPECT_THROW(expand("=no_such_cmd_xyz"), ShellError);
  shell->options.set(Option::Equals, false);
  EXPECT_EQ(expand("=sh"), "=sh");
}

}  // namespace
}  // namespace myzsh
