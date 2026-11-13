#include <gtest/gtest.h>

#include <expand/word.h>

namespace myzsh {
namespace {

using enum WordPartKind;

vec<WordPart> parts(strv raw) {
  auto result = parse_word(raw);
  EXPECT_TRUE(result) << raw;
  return result ? *result : vec<WordPart>{};
}

WordPart part(WordPartKind kind, strv text, vec<WordPart> children = {},
              bool braced = false)
{
  return WordPart{kind, str(text), s::move(children), braced};
}

TEST(ParseWord, PlainLiteral) {
  EXPECT_EQ(parts("abc"), (vec<WordPart>{part(Literal, "abc")}));
}

TEST(ParseWord, Quotes) {
  EXPECT_EQ(parts("a'b c'd"),
            (vec<WordPart>{part(Literal, "a"), part(SingleQuoted, "b c"),
                           part(Literal, "d")}));
  EXPECT_EQ(parts("$'a\\tb'"),
            (vec<WordPart>{part(DollarQuoted, "a\tb")}));
}

TEST(ParseWord, BackslashEscape) {
  EXPECT_EQ(parts("a\\*b"),
            (vec<WordPart>{part(Literal, "a"), part(Escaped, "*"),
                           part(Literal, "b")}));
}

TEST(ParseWord, DoubleQuotesHoldParts) {
  EXPECT_EQ(parts("\"x $y\""),
            (vec<WordPart>{part(DoubleQuoted, "",
                                {part(Literal, "x "), part(Param, "y")})}));
}

TEST(ParseWord, BackslashInsideDoubleQuotes) {
  // Only \$ \` \" \\ are escapes inside double quotes.
  EXPECT_EQ(parts("\"\\$a\\n\""),
            (vec<WordPart>{part(DoubleQuoted, "",
                                {part(Escaped, "$"), part(Literal, "a\\n")})}));
}

TEST(ParseWord, ParameterForms) {
  EXPECT_EQ(parts("$HOME/x"),
            (vec<WordPart>{part(Param, "HOME"), part(Literal, "/x")}));
  EXPECT_EQ(parts("${x:-y}z"),
            (vec<WordPart>{part(Param, "x:-y", {}, true), part(Literal, "z")}));
  EXPECT_EQ(parts("$12"),
            (vec<WordPart>{part(Param, "1"), part(Literal, "2")}));
  EXPECT_EQ(parts("$?$$$#"),
            (vec<WordPart>{part(Param, "?"), part(Param, "$"),
                           part(Param, "#")}));
}

TEST(ParseWord, UnbracedParameterTakesSubscript) {
  EXPECT_EQ(parts("$a[1]x"),
            (vec<WordPart>{part(Param, "a[1]"), part(Literal, "x")}));
}

TEST(ParseWord, NestedBraces) {
  EXPECT_EQ(parts("${${x#a}%b}"),
            (vec<WordPart>{part(Param, "${x#a}%b", {}, true)}));
}

TEST(ParseWord, LoneDollarIsLiteral) {
  EXPECT_EQ(parts("a$"), (vec<WordPart>{part(Literal, "a$")}));
  EXPECT_EQ(parts("$ x"), (vec<WordPart>{part(Literal, "$ x")}));
}

TEST(ParseWord, CommandSubstitution) {
  EXPECT_EQ(parts("a$(echo b)c"),
            (vec<WordPart>{part(Literal, "a"), part(CommandSubst, "echo b"),
                           part(Literal, "c")}));
  EXPECT_EQ(parts("`echo b`"),
            (vec<WordPart>{part(CommandSubst, "echo b")}));
}

TEST(ParseWord, ArithmeticSubstitution) {
  EXPECT_EQ(parts("$((1 + 2))"),
            (vec<WordPart>{part(ArithSubst, "1 + 2")}));
  EXPECT_EQ(parts("$[3*4]"), (vec<WordPart>{part(ArithSubst, "3*4")}));
}

TEST(ParseWord, ProcessSubstitution) {
  EXPECT_EQ(parts("<(ls)"), (vec<WordPart>{part(ProcSubstIn, "ls")}));
  EXPECT_EQ(parts(">(cat)"), (vec<WordPart>{part(ProcSubstOut, "cat")}));
  EXPECT_EQ(parts("=(ls)"), (vec<WordPart>{part(ProcSubstFile, "ls")}));
}

TEST(ParseWord, Errors) {
  EXPECT_FALSE(parse_word("'abc"));
  EXPECT_FALSE(parse_word("\"abc"));
  EXPECT_FALSE(parse_word("${abc"));
  EXPECT_FALSE(parse_word("$(abc"));
}

}  // namespace
}  // namespace myzsh
