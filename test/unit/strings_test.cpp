#include <gtest/gtest.h>

#include <util/strings.h>

namespace myzsh {
namespace {

using Words = vec<str>;

TEST(SplitFields, DefaultIfsCollapsesAndTrimsWhitespace) {
  EXPECT_EQ(split_fields("  a   b  ", DEFAULT_IFS), (Words{"a", "b"}));
  EXPECT_EQ(split_fields("a\tb\nc", DEFAULT_IFS), (Words{"a", "b", "c"}));
}

TEST(SplitFields, EmptyInputGivesNoFields) {
  EXPECT_EQ(split_fields("", DEFAULT_IFS), Words{});
  EXPECT_EQ(split_fields("   ", DEFAULT_IFS), Words{});
}

TEST(SplitFields, NoDelimiterGivesOneField) {
  EXPECT_EQ(split_fields("abc", DEFAULT_IFS), Words{"abc"});
}

TEST(SplitFields, EmptyIfsDisablesSplitting) {
  EXPECT_EQ(split_fields("a b", ""), Words{"a b"});
}

TEST(SplitFields, NonWhitespaceDelimitersKeepEmptyFields) {
  EXPECT_EQ(split_fields(":a::b", ":"), (Words{"", "a", "", "b"}));
}

TEST(SplitFields, TrailingNonWhitespaceDelimiterKeepsEmptyField) {
  EXPECT_EQ(split_fields("a:b:", ":"), (Words{"a", "b", ""}));
}

TEST(SplitFields, MixedWhitespaceAndNonWhitespace) {
  EXPECT_EQ(split_fields(" a : b::c ", " :"), (Words{"a", "b", "", "c"}));
}

TEST(SplitOn, DropsEmptyFieldsByDefault) {
  EXPECT_EQ(split_on("x,y,,z", ",", false), (Words{"x", "y", "z"}));
}

TEST(SplitOn, KeepsEmptyFieldsWhenAsked) {
  EXPECT_EQ(split_on("x,y,,z", ",", true), (Words{"x", "y", "", "z"}));
}

TEST(SplitOn, MultiCharacterSeparator) {
  EXPECT_EQ(split_on("a--b--c", "--", false), (Words{"a", "b", "c"}));
}

TEST(SplitOn, EmptySeparatorSplitsCharacters) {
  EXPECT_EQ(split_on("abc", "", false), (Words{"a", "b", "c"}));
}

TEST(Join, JoinsWithSeparator) {
  EXPECT_EQ(join({"a", "b", "c"}, ","), "a,b,c");
  EXPECT_EQ(join({}, ","), "");
  EXPECT_EQ(join({"x"}, ", "), "x");
}

TEST(IsIdentifier, AcceptsShellNames) {
  EXPECT_TRUE(is_identifier("a"));
  EXPECT_TRUE(is_identifier("_x1"));
  EXPECT_TRUE(is_identifier("PATH"));
  EXPECT_FALSE(is_identifier(""));
  EXPECT_FALSE(is_identifier("1a"));
  EXPECT_FALSE(is_identifier("a-b"));
  EXPECT_FALSE(is_identifier("a b"));
}

TEST(IsIfsWhitespace, RecognisesBlankCharacters) {
  EXPECT_TRUE(is_ifs_whitespace(' '));
  EXPECT_TRUE(is_ifs_whitespace('\t'));
  EXPECT_TRUE(is_ifs_whitespace('\n'));
  EXPECT_FALSE(is_ifs_whitespace(':'));
  EXPECT_FALSE(is_ifs_whitespace('a'));
}

TEST(CaseConversion, LowerUpperCapitalize) {
  EXPECT_EQ(to_lower("ABC def"), "abc def");
  EXPECT_EQ(to_upper("abc DEF"), "ABC DEF");
  EXPECT_EQ(capitalize_words("hello wORLD foo-bar"), "Hello World Foo-Bar");
}

struct QuoteCase {
  str input;
  QuoteStyle style;
  str expected;
};

void PrintTo(const QuoteCase& param, s::ostream* out) {
  *out << '"' << param.input << "\" style " << static_cast<int>(param.style);
}

class QuoteTest : public ::testing::TestWithParam<QuoteCase> {};

TEST_P(QuoteTest, MatchesZshOutput) {
  const QuoteCase& param = GetParam();
  EXPECT_EQ(quote(param.input, param.style), param.expected);
}

// Expected values captured from zsh 5.9: print -r -- ${(q)x} etc.
INSTANTIATE_TEST_SUITE_P(Styles, QuoteTest, ::testing::Values(
  QuoteCase{" a : b::c ", QuoteStyle::Backslash, "\\ a\\ :\\ b::c\\ "},
  QuoteCase{" a : b::c ", QuoteStyle::Single, "' a : b::c '"},
  QuoteCase{" a : b::c ", QuoteStyle::Double, "\" a : b::c \""},
  QuoteCase{" a : b::c ", QuoteStyle::Dollar, "$' a : b::c '"},
  QuoteCase{" a : b::c ", QuoteStyle::MinimalSingle, "' a : b::c '"},
  QuoteCase{" a : b::c ", QuoteStyle::MinimalDollar, "' a : b::c '"},
  QuoteCase{"a b\tc", QuoteStyle::Backslash, "a\\ b$'\\t'c"},
  QuoteCase{"a b\tc", QuoteStyle::Single, "'a b\tc'"},
  QuoteCase{"a b\tc", QuoteStyle::Double, "\"a b\tc\""},
  QuoteCase{"a b\tc", QuoteStyle::Dollar, "$'a b\\tc'"},
  QuoteCase{"a b\tc", QuoteStyle::MinimalSingle, "'a b\tc'"},
  QuoteCase{"a b\tc", QuoteStyle::MinimalDollar, "$'a b\\tc'"},
  QuoteCase{"it's", QuoteStyle::Backslash, "it\\'s"},
  QuoteCase{"it's", QuoteStyle::Single, "'it'\\''s'"},
  QuoteCase{"it's", QuoteStyle::Double, "\"it's\""},
  QuoteCase{"it's", QuoteStyle::Dollar, "$'it\\'s'"},
  QuoteCase{"it's", QuoteStyle::MinimalSingle, "it\\'s"},
  QuoteCase{"it's", QuoteStyle::MinimalDollar, "'it'\\''s'"},
  QuoteCase{"abc", QuoteStyle::MinimalSingle, "abc"},
  QuoteCase{"abc", QuoteStyle::MinimalDollar, "abc"},
  QuoteCase{"*?[]", QuoteStyle::Backslash, "\\*\\?\\[\\]"}));

TEST(Unquote, RemovesOneLevelOfQuoting) {
  EXPECT_EQ(unquote("'a b'"), "a b");
  EXPECT_EQ(unquote("a\\ b"), "a b");
  EXPECT_EQ(unquote("\"x y\""), "x y");
  EXPECT_EQ(unquote("$'a\\tb'"), "a\tb");
  EXPECT_EQ(unquote("plain"), "plain");
}

TEST(Unquote, InvertsQuote) {
  for (strv text : {"it's", "a b\tc", " $x ", "back\\slash"}) {
    for (QuoteStyle style : {QuoteStyle::Backslash, QuoteStyle::Single,
                             QuoteStyle::Double, QuoteStyle::Dollar}) {
      EXPECT_EQ(unquote(quote(text, style)), text) << text;
    }
  }
}

TEST(InterpretEscapes, PrintStyle) {
  EXPECT_EQ(interpret_escapes("a\\tb", false).text, "a\tb");
  EXPECT_EQ(interpret_escapes("\\x41\\101", false).text, "AA");
  // \0NNN takes at most three octal digits after the 0.
  EXPECT_EQ(interpret_escapes("\\0101", false).text, "\b1");
  EXPECT_EQ(interpret_escapes("\\e\\a\\n\\\\", false).text, "\x1b\a\n\\");
  EXPECT_EQ(interpret_escapes("\\u00e9", false).text, "\xc3\xa9");
}

TEST(InterpretEscapes, EchoStyleNeedsLeadingZeroForOctal) {
  EXPECT_EQ(interpret_escapes("a\\101b", true).text, "a\\101b");
  EXPECT_EQ(interpret_escapes("\\0101", true).text, "A");
}

TEST(InterpretEscapes, BackslashCStopsOutput) {
  EscapeResult result = interpret_escapes("x\\cy", false);
  EXPECT_EQ(result.text, "x");
  EXPECT_TRUE(result.stop);
}

TEST(InterpretEscapes, UnknownEscapeIsKept) {
  EXPECT_EQ(interpret_escapes("\\q", false).text, "\\q");
}

TEST(ParseDecimal, ParsesSignedIntegers) {
  EXPECT_EQ(parse_decimal("42"), 42);
  EXPECT_EQ(parse_decimal("-7"), -7);
  EXPECT_EQ(parse_decimal("+3"), 3);
  EXPECT_EQ(parse_decimal(""), s::nullopt);
  EXPECT_EQ(parse_decimal("4x"), s::nullopt);
  EXPECT_EQ(parse_decimal("99999999999999999999"), s::nullopt);
}

}  // namespace
}  // namespace myzsh
