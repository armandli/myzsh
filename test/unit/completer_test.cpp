#include <gtest/gtest.h>

#include <algorithm>

#include <complete/completer.h>
#include <support/test_support.h>

namespace myzsh {
namespace {

struct CompleterTest : ::testing::Test {
  uptr<Shell> shell = test::make_shell();
  Completer completer{*shell};
  test::TempDir dir;
  opt<test::ScopedCwd> cwd;

  void SetUp() override {
    dir.write("alpha.txt");
    dir.write("alpine.txt");
    dir.write(".dotfile");
    dir.mkdir("albums");
    cwd.emplace(dir.path());
  }

  CompletionResult complete(strv buffer) {
    return completer.complete({str(buffer), buffer.size()});
  }
};

TEST_F(CompleterTest, ClassifyContexts) {
  EXPECT_EQ(completer.classify({"ec", 2}), CompletionContext::Command);
  EXPECT_EQ(completer.classify({"ls al", 5}), CompletionContext::Argument);
  EXPECT_EQ(completer.classify({"ls | gr", 7}), CompletionContext::Command);
  EXPECT_EQ(completer.classify({"echo $HO", 8}), CompletionContext::Parameter);
  EXPECT_EQ(completer.classify({"cd ~ro", 6}), CompletionContext::Tilde);
  EXPECT_EQ(completer.classify({"setopt ext", 10}), CompletionContext::Option);
  EXPECT_EQ(completer.classify({"ls > al", 7}), CompletionContext::Redirect);
}

TEST_F(CompleterTest, Files) {
  CompletionResult result = complete("cat alp");
  EXPECT_EQ(result.word_start, 4u);
  EXPECT_EQ(result.word_end, 7u);
  EXPECT_EQ(result.matches, (vec<str>{"alpha.txt", "alpine.txt"}));
  EXPECT_EQ(result.common_prefix, "alp");
}

TEST_F(CompleterTest, DirectoriesOnlyForCd) {
  CompletionResult result = complete("cd al");
  EXPECT_EQ(result.matches, vec<str>{"albums"});
  EXPECT_EQ(result.suffixes, vec<str>{"/"});
}

TEST_F(CompleterTest, HiddenFilesNeedDot) {
  EXPECT_TRUE(completer.complete_files("", false).size() == 3u);
  EXPECT_EQ(completer.complete_files(".d", false), vec<str>{".dotfile"});
}

TEST_F(CompleterTest, Commands) {
  vec<str> matches = completer.complete_commands("pri");
  EXPECT_NE(s::ranges::find(matches, "print"), matches.end());
  EXPECT_NE(s::ranges::find(matches, "printf"), matches.end());
  shell->run_string("primary_fn() { }");
  matches = completer.complete_commands("prim");
  EXPECT_NE(s::ranges::find(matches, "primary_fn"), matches.end());
}

TEST_F(CompleterTest, Parameters) {
  shell->params.set_scalar("MYVAR_ONE", "1");
  shell->params.set_scalar("MYVAR_TWO", "2");
  EXPECT_EQ(completer.complete_parameters("MYVAR"),
            (vec<str>{"MYVAR_ONE", "MYVAR_TWO"}));
}

TEST_F(CompleterTest, Options) {
  vec<str> matches = completer.complete_options("extended");
  EXPECT_NE(s::ranges::find(matches, "extendedglob"), matches.end());
  EXPECT_NE(s::ranges::find(matches, "extendedhistory"), matches.end());
}

TEST(CompletionHelpers, LongestCommonPrefix) {
  EXPECT_EQ(longest_common_prefix({"alpha", "alpine", "alps"}), "alp");
  EXPECT_EQ(longest_common_prefix({"x"}), "x");
  EXPECT_EQ(longest_common_prefix({}), "");
  EXPECT_EQ(longest_common_prefix({"a", "b"}), "");
}

TEST(CompletionHelpers, FormatColumns) {
  EXPECT_EQ(format_columns({"a", "b", "c", "d"}, 7, false),
            (vec<str>{"a  c", "b  d"}));
  EXPECT_EQ(format_columns({"a", "b", "c", "d"}, 7, true),
            (vec<str>{"a  b", "c  d"}));
  EXPECT_EQ(format_columns({"long-name", "x"}, 5, false),
            (vec<str>{"long-name", "x"}));
}

}  // namespace
}  // namespace myzsh
