#include <gtest/gtest.h>

#include <unistd.h>

#include <support/test_support.h>
#include <util/error.h>

namespace myzsh {
namespace {

struct SpecialParamTest : ::testing::Test {
  uptr<Shell> shell = test::make_shell();
  ParamTable& params = shell->params;
};

TEST_F(SpecialParamTest, StatusAndPid) {
  shell->status = 3;
  EXPECT_EQ(params.get_scalar("?"), "3");
  EXPECT_EQ(params.get_scalar("status"), "3");
  EXPECT_EQ(params.get_scalar("$"), s::to_string(getpid()));
}

TEST_F(SpecialParamTest, PositionalParameters) {
  shell->positional = {"a", "b c", "d"};
  EXPECT_EQ(params.get_scalar("#"), "3");
  EXPECT_EQ(params.get_scalar("1"), "a");
  EXPECT_EQ(params.get_scalar("2"), "b c");
  EXPECT_EQ(params.get_array("@"), (vec<str>{"a", "b c", "d"}));
  EXPECT_EQ(params.get_array("argv"), (vec<str>{"a", "b c", "d"}));
  EXPECT_EQ(params.get_scalar("*"), "a b c d");
  EXPECT_EQ(params.get_scalar("4"), s::nullopt);
}

TEST_F(SpecialParamTest, AssigningArgvSetsPositionals) {
  params.set_array("argv", {"x", "y"});
  EXPECT_EQ(shell->positional, (vec<str>{"x", "y"}));
}

TEST_F(SpecialParamTest, ArgZero) {
  shell->argzero = "myscript";
  EXPECT_EQ(params.get_scalar("0"), "myscript");
}

TEST_F(SpecialParamTest, RandomIsInRangeAndSeedable) {
  for (int i = 0; i < 100; ++i) {
    auto value = params.get_integer("RANDOM");
    ASSERT_TRUE(value);
    EXPECT_GE(*value, 0);
    EXPECT_LE(*value, 32767);
  }
  params.set_integer("RANDOM", 42);
  auto first = params.get_integer("RANDOM");
  params.set_integer("RANDOM", 42);
  EXPECT_EQ(params.get_integer("RANDOM"), first);
}

TEST_F(SpecialParamTest, SecondsCanBeReset) {
  params.set_integer("SECONDS", 100);
  auto value = params.get_integer("SECONDS");
  ASSERT_TRUE(value);
  EXPECT_GE(*value, 100);
  EXPECT_LE(*value, 101);
}

TEST_F(SpecialParamTest, LinenoTracksShell) {
  shell->lineno = 17;
  EXPECT_EQ(params.get_scalar("LINENO"), "17");
}

TEST_F(SpecialParamTest, PipestatusIsArray) {
  shell->pipestatus = {0, 1, 2};
  EXPECT_EQ(params.get_array("pipestatus"), (vec<str>{"0", "1", "2"}));
}

TEST_F(SpecialParamTest, DashReflectsOptions) {
  shell->options.set(Option::Xtrace, true);
  auto dash = params.get_scalar("-");
  ASSERT_TRUE(dash);
  EXPECT_NE(dash->find('x'), str::npos);
}

TEST_F(SpecialParamTest, TiedPathArrays) {
  params.set_scalar("PATH", "/bin:/usr/bin");
  EXPECT_EQ(params.get_array("path"), (vec<str>{"/bin", "/usr/bin"}));
  params.set_array("fpath", {"/f1", "/f2"});
  EXPECT_EQ(params.get_scalar("FPATH"), "/f1:/f2");
  params.set_scalar("CDPATH", ".:/tmp");
  EXPECT_EQ(params.get_array("cdpath"), (vec<str>{".", "/tmp"}));
}

TEST_F(SpecialParamTest, AssigningPathClearsCommandHash) {
  shell->command_hash.add("fake", "/nowhere/fake");
  params.set_scalar("PATH", "/bin");
  EXPECT_TRUE(shell->command_hash.entries().empty());
}

TEST_F(SpecialParamTest, ReadOnlySpecials) {
  EXPECT_THROW(params.set_scalar("$", "1"), ShellError);
  EXPECT_THROW(params.set_scalar("PPID", "1"), ShellError);
}

TEST_F(SpecialParamTest, VersionAndIdentity) {
  EXPECT_TRUE(params.is_set("ZSH_VERSION"));
  EXPECT_EQ(params.get_scalar("UID"), s::to_string(getuid()));
  EXPECT_EQ(params.get_scalar("EUID"), s::to_string(geteuid()));
  EXPECT_EQ(params.get_scalar("PPID"), s::to_string(getppid()));
}

TEST_F(SpecialParamTest, ZshSubshell) {
  shell->subshell_level = 2;
  EXPECT_EQ(params.get_scalar("ZSH_SUBSHELL"), "2");
}

TEST_F(SpecialParamTest, LastBackgroundPid) {
  shell->last_background = 4242;
  EXPECT_EQ(params.get_scalar("!"), "4242");
}

TEST_F(SpecialParamTest, IfsDefault) {
  EXPECT_EQ(params.get_scalar("IFS"), str(" \t\n\0", 4));
}

}  // namespace
}  // namespace myzsh
