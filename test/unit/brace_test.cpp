#include <gtest/gtest.h>

#include <expand/brace.h>

namespace myzsh {
namespace {

vec<str> expand(strv word, bool brace_ccl = false) {
  vec<str> result;
  for (const QString& expanded : expand_braces(QString(word), brace_ccl)) {
    result.push_back(expanded.text);
  }
  return result;
}

using Words = vec<str>;

// Expected values from zsh 5.9.
TEST(BraceExpansion, CommaLists) {
  EXPECT_EQ(expand("{a,b}{1,2}"), (Words{"a1", "a2", "b1", "b2"}));
  EXPECT_EQ(expand("x{,y}z"), (Words{"xz", "xyz"}));
  EXPECT_EQ(expand("pre{a,b,c}post"),
            (Words{"preapost", "prebpost", "precpost"}));
}

TEST(BraceExpansion, Nested) {
  EXPECT_EQ(expand("{a,{b,c}d}"), (Words{"a", "bd", "cd"}));
}

TEST(BraceExpansion, NumericRanges) {
  EXPECT_EQ(expand("{1..5}"), (Words{"1", "2", "3", "4", "5"}));
  EXPECT_EQ(expand("{3..1}"), (Words{"3", "2", "1"}));
  EXPECT_EQ(expand("{05..10..2}"), (Words{"05", "07", "09"}));
  EXPECT_EQ(expand("{-2..2}"), (Words{"-2", "-1", "0", "1", "2"}));
  EXPECT_EQ(expand("{1..10..-3}"), (Words{"10", "7", "4", "1"}));
}

TEST(BraceExpansion, CharacterRanges) {
  EXPECT_EQ(expand("{a..e}"), (Words{"a", "b", "c", "d", "e"}));
  EXPECT_EQ(expand("{e..c}"), (Words{"e", "d", "c"}));
}

TEST(BraceExpansion, NotABraceExpansion) {
  EXPECT_EQ(expand("{abc}"), Words{"{abc}"});
  EXPECT_EQ(expand("{}"), Words{"{}"});
  EXPECT_EQ(expand("a{b"), Words{"a{b"});
}

TEST(BraceExpansion, BraceCcl) {
  EXPECT_EQ(expand("{abc}", true), (Words{"a", "b", "c"}));
  EXPECT_EQ(expand("{a-c}x", true), (Words{"ax", "bx", "cx"}));
}

TEST(BraceExpansion, QuotedBracesAreLiteral) {
  QString word;
  word.append("{a,b}", true);
  vec<QString> result = expand_braces(word, false);
  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(result[0].text, "{a,b}");
}

TEST(BraceExpansion, QuotedCommaDoesNotSplit) {
  QString word;
  word.append("{a", false);
  word.append(",", true);
  word.append("b,c}", false);
  vec<str> texts;
  for (const QString& expanded : expand_braces(word, false)) {
    texts.push_back(expanded.text);
  }
  EXPECT_EQ(texts, (Words{"a,b", "c"}));
}

}  // namespace
}  // namespace myzsh
