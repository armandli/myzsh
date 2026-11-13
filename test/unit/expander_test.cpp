#include <gtest/gtest.h>

#include <expand/expander.h>
#include <support/test_support.h>
#include <util/error.h>

namespace myzsh {
namespace {

using Words = vec<str>;

struct ExpanderTest : ::testing::Test {
  uptr<Shell> shell = test::make_shell();
  Expander expander{*shell};
  test::TempDir dir;

  void SetUp() override {
    shell->params.set_scalar("x", "a b");
    shell->params.set_scalar("empty", "");
    shell->params.set_array("arr", {"one", "two words", "three"});
    shell->params.set_array("none", {});
    shell->params.set_scalar("HOME", "/home/me");
    shell->positional = {"p1", "p 2"};
  }

  Words words(strv raw) { return expander.expand_word(raw); }
};

TEST_F(ExpanderTest, LiteralAndQuoteRemoval) {
  EXPECT_EQ(words("plain"), Words{"plain"});
  EXPECT_EQ(words("'a b'"), Words{"a b"});
  EXPECT_EQ(words("\"a b\""), Words{"a b"});
  EXPECT_EQ(words("a\\ b"), Words{"a b"});
  EXPECT_EQ(words("$'a\\tb'"), Words{"a\tb"});
}

TEST_F(ExpanderTest, NoWordSplittingByDefault) {
  EXPECT_EQ(words("$x"), Words{"a b"});
}

TEST_F(ExpanderTest, ShWordSplitOption) {
  shell->options.set(Option::Shwordsplit, true);
  EXPECT_EQ(words("$x"), (Words{"a", "b"}));
  EXPECT_EQ(words("\"$x\""), Words{"a b"});
}

TEST_F(ExpanderTest, ExplicitSplitFlag) {
  EXPECT_EQ(words("${=x}"), (Words{"a", "b"}));
}

TEST_F(ExpanderTest, EmptyUnquotedWordsAreRemoved) {
  EXPECT_EQ(words("$empty"), Words{});
  EXPECT_EQ(words("$unset_var"), Words{});
  EXPECT_EQ(words("\"$empty\""), Words{""});
  EXPECT_EQ(words("''"), Words{""});
}

TEST_F(ExpanderTest, ArraysExpandToSeparateWords) {
  EXPECT_EQ(words("$arr"), (Words{"one", "two words", "three"}));
  EXPECT_EQ(words("\"$arr\""), Words{"one two words three"});
  EXPECT_EQ(words("\"${arr[@]}\""), (Words{"one", "two words", "three"}));
  EXPECT_EQ(words("$none"), Words{});
}

TEST_F(ExpanderTest, PositionalParameters) {
  EXPECT_EQ(words("\"$@\""), (Words{"p1", "p 2"}));
  EXPECT_EQ(words("\"$*\""), Words{"p1 p 2"});
  EXPECT_EQ(words("$#"), Words{"2"});
  shell->positional.clear();
  EXPECT_EQ(words("\"$@\""), Words{});
}

TEST_F(ExpanderTest, ArrayWithSurroundingText) {
  EXPECT_EQ(words("x${arr}y"), (Words{"xone", "two words", "threey"}));
}

TEST_F(ExpanderTest, RcExpandParam) {
  EXPECT_EQ(words("x${^arr}y"),
            (Words{"xoney", "xtwo wordsy", "xthreey"}));
  shell->options.set(Option::Rcexpandparam, true);
  EXPECT_EQ(words("<$arr>"), (Words{"<one>", "<two words>", "<three>"}));
}

TEST_F(ExpanderTest, CommandSubstitution) {
  EXPECT_EQ(words("$(echo hi)"), Words{"hi"});
  EXPECT_EQ(words("`echo hi`"), Words{"hi"});
  EXPECT_EQ(expander.command_substitution("printf 'a\\n\\n\\n'"), "a");
  // Unquoted results are split on IFS; quoted ones are not.
  EXPECT_EQ(words("$(echo a b)"), (Words{"a", "b"}));
  EXPECT_EQ(words("\"$(echo a b)\""), Words{"a b"});
}

TEST_F(ExpanderTest, CommandSubstitutionSetsStatus) {
  words("$(exit 3)");
  EXPECT_EQ(shell->status, 3);
}

TEST_F(ExpanderTest, ArithmeticExpansion) {
  EXPECT_EQ(words("$((1 + 2))"), Words{"3"});
  EXPECT_EQ(words("$[2 * 3]"), Words{"6"});
  EXPECT_EQ(words("x$((7 / 2.0))"), Words{"x3.5"});
}

TEST_F(ExpanderTest, BraceExpansionHappensAfterParameters) {
  shell->params.set_scalar("v", "1");
  EXPECT_EQ(words("a{$v,2}"), (Words{"a1", "a2"}));
  EXPECT_EQ(words("'{a,b}'"), Words{"{a,b}"});
  shell->options.set(Option::Ignorebraces, true);
  EXPECT_EQ(words("{a,b}"), Words{"{a,b}"});
}

TEST_F(ExpanderTest, TildeExpansion) {
  EXPECT_EQ(words("~/x"), Words{"/home/me/x"});
  EXPECT_EQ(words("'~'/x"), Words{"~/x"});
}

TEST_F(ExpanderTest, Globbing) {
  dir.write("f1.c");
  dir.write("f2.c");
  dir.write("g.h");
  test::ScopedCwd cwd(dir.path());
  EXPECT_EQ(words("*.c"), (Words{"f1.c", "f2.c"}));
  EXPECT_EQ(words("'*'.c"), Words{"*.c"});
  shell->params.set_scalar("pat", "*.c");
  // Parameter values are not globbed unless GLOB_SUBST or ${~...}.
  EXPECT_EQ(words("$pat"), Words{"*.c"});
  EXPECT_EQ(words("${~pat}"), (Words{"f1.c", "f2.c"}));
}

TEST_F(ExpanderTest, NoMatchIsAnError) {
  test::ScopedCwd cwd(dir.path());
  try {
    words("*.nomatch");
    FAIL();
  } catch (const ShellError& error) {
    EXPECT_STREQ(error.what(), "no matches found: *.nomatch");
  }
}

TEST_F(ExpanderTest, NullGlobAndNoNomatch) {
  test::ScopedCwd cwd(dir.path());
  shell->options.set(Option::Nullglob, true);
  EXPECT_EQ(words("*.nomatch"), Words{});
  shell->options.set(Option::Nullglob, false);
  shell->options.set(Option::Nomatch, false);
  EXPECT_EQ(words("*.nomatch"), Words{"*.nomatch"});
}

TEST_F(ExpanderTest, NoGlobOption) {
  test::ScopedCwd cwd(dir.path());
  dir.write("a.c");
  shell->options.set(Option::Glob, false);
  EXPECT_EQ(words("*.c"), Words{"*.c"});
}

TEST_F(ExpanderTest, ExpandWordsConcatenates) {
  vec<ast::Word> input{{"a", 1}, {"$arr", 1}, {"$empty", 1}, {"b", 1}};
  EXPECT_EQ(expander.expand_words(input),
            (Words{"a", "one", "two words", "three", "b"}));
}

TEST_F(ExpanderTest, AssignmentContext) {
  EXPECT_EQ(expander.expand_assignment("$x"), "a b");
  EXPECT_EQ(expander.expand_assignment("~/a:~/b"), "/home/me/a:/home/me/b");
  dir.write("q.c");
  test::ScopedCwd cwd(dir.path());
  EXPECT_EQ(expander.expand_assignment("*.c"), "*.c");
}

TEST_F(ExpanderTest, ScalarContextJoinsArrays) {
  EXPECT_EQ(expander.expand_scalar("$arr"), "one two words three");
  EXPECT_EQ(expander.expand_scalar("'a'\"b\""), "ab");
}

TEST_F(ExpanderTest, PatternContextKeepsQuoting) {
  QString pattern = expander.expand_pattern("a'*'$x*");
  EXPECT_EQ(pattern.text, "a*a b*");
  EXPECT_FALSE(pattern.is_quoted(0));
  EXPECT_TRUE(pattern.is_quoted(1));
  // Text from $x is literal without GLOB_SUBST.
  EXPECT_TRUE(pattern.is_quoted(3));
  EXPECT_FALSE(pattern.is_quoted(pattern.size() - 1));
}

TEST_F(ExpanderTest, HereDocBody) {
  EXPECT_EQ(expander.expand_heredoc("x=$x '$x' \\$x $((1+1))\n"),
            "x=a b 'a b' $x 2\n");
}

TEST_F(ExpanderTest, ExpandString) {
  EXPECT_EQ(expander.expand_string("v=$x; $(echo c)"), "v=a b; c");
}

TEST_F(ExpanderTest, ProcessSubstitutionGivesReadablePath) {
  str path = expander.process_substitution("echo from-proc", true, false);
  EXPECT_EQ(path.rfind("/dev/fd/", 0), 0u);
  str file = expander.process_substitution("echo from-file", true, true);
  EXPECT_EQ(test::read_file(file), "from-file\n");
}

}  // namespace
}  // namespace myzsh
