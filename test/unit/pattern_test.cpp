#include <gtest/gtest.h>

#include <pattern/pattern.h>

namespace myzsh {
namespace {

PatternOptions plain() { return {}; }

PatternOptions extended() {
  PatternOptions options;
  options.extended_glob = true;
  return options;
}

PatternOptions ksh() {
  PatternOptions options;
  options.ksh_glob = true;
  return options;
}

bool match(strv pattern, strv text, PatternOptions options = plain()) {
  auto compiled = Pattern::compile(pattern, options);
  EXPECT_TRUE(compiled) << pattern;
  return compiled and compiled->matches(text);
}

TEST(Pattern, Literal) {
  EXPECT_TRUE(match("abc", "abc"));
  EXPECT_FALSE(match("abc", "abcd"));
  EXPECT_FALSE(match("abc", "ab"));
  EXPECT_TRUE(match("", ""));
}

TEST(Pattern, StarAndQuestion) {
  EXPECT_TRUE(match("a*", "abc"));
  EXPECT_TRUE(match("a*", "a"));
  EXPECT_TRUE(match("*c", "abc"));
  EXPECT_TRUE(match("a?c", "abc"));
  EXPECT_FALSE(match("a?c", "ac"));
  EXPECT_TRUE(match("*", ""));
  EXPECT_TRUE(match("a*b*c", "aXXbYYc"));
  EXPECT_FALSE(match("a*b*c", "aXXbYY"));
}

TEST(Pattern, BracketExpressions) {
  EXPECT_TRUE(match("[abc]", "b"));
  EXPECT_FALSE(match("[abc]", "d"));
  EXPECT_TRUE(match("[a-c]x", "bx"));
  EXPECT_TRUE(match("[!a]", "b"));
  EXPECT_TRUE(match("[^a]", "b"));
  EXPECT_FALSE(match("[^a]", "a"));
  EXPECT_TRUE(match("[]]", "]"));
  EXPECT_TRUE(match("[a-]", "-"));
}

TEST(Pattern, CharacterClasses) {
  EXPECT_TRUE(match("[[:alpha:]]", "q"));
  EXPECT_FALSE(match("[[:alpha:]]", "1"));
  EXPECT_TRUE(match("[[:digit:]][[:space:]]", "1 "));
  EXPECT_TRUE(match("[[:upper:][:digit:]]", "Z"));
  EXPECT_TRUE(match("[[:alnum:]_]", "_"));
  EXPECT_TRUE(match("[[:punct:]]", "!"));
  EXPECT_TRUE(match("[[:xdigit:]]", "f"));
}

TEST(Pattern, NumericRanges) {
  EXPECT_TRUE(match("file<1-5>", "file3"));
  EXPECT_FALSE(match("file<1-5>", "file10"));
  EXPECT_TRUE(match("<->", "12345"));
  EXPECT_TRUE(match("x<10->", "x99"));
  EXPECT_FALSE(match("x<10->", "x9"));
  EXPECT_TRUE(match("<-5>", "05"));
}

TEST(Pattern, Alternation) {
  EXPECT_TRUE(match("(a|b).c", "a.c"));
  EXPECT_TRUE(match("(a|b).c", "b.c"));
  EXPECT_FALSE(match("(a|b).c", "c.c"));
  EXPECT_TRUE(match("x(foo|ba*)y", "xbarrry"));
}

TEST(Pattern, QuotedCharactersAreLiteral) {
  QString pattern;
  pattern.append("a", false);
  pattern.append("*", true);
  auto compiled = Pattern::compile(pattern, plain());
  ASSERT_TRUE(compiled);
  EXPECT_TRUE(compiled->matches("a*"));
  EXPECT_FALSE(compiled->matches("abc"));
  EXPECT_TRUE(compiled->is_literal());
}

TEST(Pattern, BackslashEscapes) {
  EXPECT_TRUE(match("a\\*", "a*"));
  EXPECT_FALSE(match("a\\*", "ab"));
}

TEST(Pattern, IsLiteral) {
  EXPECT_TRUE(Pattern::compile("abc", plain())->is_literal());
  EXPECT_FALSE(Pattern::compile("a*c", plain())->is_literal());
}

TEST(Pattern, SyntaxErrors) {
  EXPECT_FALSE(Pattern::compile("(abc", plain()));
  EXPECT_FALSE(Pattern::compile("(#q", extended()));
}

TEST(Pattern, UnterminatedBracketIsLiteral) {
  // zsh treats a lone '[' as an ordinary character.
  EXPECT_TRUE(match("a[b", "a[b"));
}

TEST(PatternExtended, Negation) {
  EXPECT_TRUE(match("^*.c", "x.h", extended()));
  EXPECT_FALSE(match("^*.c", "x.c", extended()));
  // Without EXTENDED_GLOB '^' is an ordinary character.
  EXPECT_FALSE(match("^*.c", "x.h", plain()));
}

TEST(PatternExtended, Exclusion) {
  EXPECT_TRUE(match("*~*.h", "a.c", extended()));
  EXPECT_FALSE(match("*~*.h", "a.h", extended()));
  EXPECT_TRUE(match("*.c~b*", "a.c", extended()));
  EXPECT_FALSE(match("*.c~b*", "b.c", extended()));
}

TEST(PatternExtended, Repetition) {
  EXPECT_TRUE(match("a#", "", extended()));
  EXPECT_TRUE(match("a#", "aaa", extended()));
  EXPECT_FALSE(match("a##", "", extended()));
  EXPECT_TRUE(match("a##b", "aaab", extended()));
  EXPECT_TRUE(match("(ab)#", "ababab", extended()));
  EXPECT_FALSE(match("(ab)#", "aba", extended()));
}

TEST(PatternExtended, CaseInsensitiveFlags) {
  EXPECT_TRUE(match("(#i)A.C", "a.c", extended()));
  EXPECT_TRUE(match("(#i)abc", "ABC", extended()));
  EXPECT_TRUE(match("a(#i)bc", "aBC", extended()));
  EXPECT_FALSE(match("a(#i)bc", "ABC", extended()));
  // (#l): lower case in the pattern matches either case.
  EXPECT_TRUE(match("(#l)abc", "ABC", extended()));
  EXPECT_FALSE(match("(#l)ABC", "abc", extended()));
  EXPECT_TRUE(match("(#i)a(#I)b", "Ab", extended()));
  EXPECT_FALSE(match("(#i)a(#I)b", "AB", extended()));
}

TEST(PatternExtended, Approximate) {
  EXPECT_TRUE(match("(#a1)abc", "abd", extended()));
  EXPECT_TRUE(match("(#a1)abc", "ab", extended()));
  EXPECT_TRUE(match("(#a1)abc", "abxc", extended()));
  EXPECT_TRUE(match("(#a1)abc", "acb", extended()));
  EXPECT_FALSE(match("(#a1)abc", "xyz", extended()));
  EXPECT_TRUE(match("(#a2)abcd", "axcy", extended()));
}

TEST(PatternExtended, Anchors) {
  auto start = Pattern::compile("(#s)ab", extended());
  ASSERT_TRUE(start);
  auto at_start = start->search("ab xab", 0, false);
  ASSERT_TRUE(at_start);
  EXPECT_EQ(at_start->span, (MatchSpan{0, 2}));
  EXPECT_FALSE(start->search("xab", 0, false));
  auto end = Pattern::compile("ab(#e)", extended());
  ASSERT_TRUE(end);
  auto at_end = end->search("ab xab", 0, false);
  ASSERT_TRUE(at_end);
  EXPECT_EQ(at_end->span, (MatchSpan{4, 6}));
}

TEST(PatternExtended, Backreferences) {
  auto pattern = Pattern::compile("(#b)(*).(*)", extended());
  ASSERT_TRUE(pattern);
  auto result = pattern->match("foo.tar");
  ASSERT_TRUE(result);
  ASSERT_EQ(result->captures.size(), 2u);
  ASSERT_TRUE(result->captures[0]);
  EXPECT_EQ(*result->captures[0], (MatchSpan{0, 3}));
  ASSERT_TRUE(result->captures[1]);
  EXPECT_EQ(*result->captures[1], (MatchSpan{4, 7}));
}

TEST(PatternExtended, UnmatchedAlternativeCaptureIsEmpty) {
  auto pattern = Pattern::compile("(#b)((a)|(b))", extended());
  ASSERT_TRUE(pattern);
  auto result = pattern->match("b");
  ASSERT_TRUE(result);
  ASSERT_EQ(result->captures.size(), 3u);
  EXPECT_EQ(result->captures[0], (MatchSpan{0, 1}));
  EXPECT_FALSE(result->captures[1]);
  EXPECT_EQ(result->captures[2], (MatchSpan{0, 1}));
}

TEST(PatternKsh, Operators) {
  EXPECT_TRUE(match("@(a|c).*", "a.c", ksh()));
  EXPECT_FALSE(match("@(a|c).*", "b.c", ksh()));
  EXPECT_TRUE(match("+(file)1", "filefile1", ksh()));
  EXPECT_FALSE(match("+(file)1", "1", ksh()));
  EXPECT_TRUE(match("*(ab)", "", ksh()));
  EXPECT_TRUE(match("*(ab)", "abab", ksh()));
  EXPECT_TRUE(match("?(x)y", "y", ksh()));
  EXPECT_TRUE(match("?(x)y", "xy", ksh()));
  EXPECT_TRUE(match("!(*.c)", "a.h", ksh()));
  EXPECT_FALSE(match("!(*.c)", "a.c", ksh()));
}

TEST(PatternOptionsTest, CaseInsensitiveOption) {
  PatternOptions options;
  options.case_insensitive = true;
  EXPECT_TRUE(match("abc", "ABC", options));
}

TEST(PatternOptionsTest, ShGlobDisablesParens) {
  PatternOptions options;
  options.sh_glob = true;
  EXPECT_TRUE(match("(a|b)", "(a|b)", options));
  EXPECT_FALSE(match("(a|b)", "a", options));
}

TEST(PatternOptionsTest, FileModeSlashAndDot) {
  PatternOptions options;
  options.file_mode = true;
  EXPECT_FALSE(match("*", "a/b", options));
  EXPECT_FALSE(match("a?b", "a/b", options));
  EXPECT_FALSE(match("*", ".hidden", options));
  EXPECT_TRUE(match(".*", ".hidden", options));
  options.glob_dots = true;
  EXPECT_TRUE(match("*", ".hidden", options));
}

TEST(PatternMatching, PrefixShortestAndLongest) {
  auto pattern = Pattern::compile("*/", plain());
  ASSERT_TRUE(pattern);
  strv path = "/usr/local/lib";
  auto shortest = pattern->match_prefix(path, false);
  ASSERT_TRUE(shortest);
  EXPECT_EQ(shortest->span, (MatchSpan{0, 1}));
  auto longest = pattern->match_prefix(path, true);
  ASSERT_TRUE(longest);
  EXPECT_EQ(longest->span, (MatchSpan{0, 11}));
  EXPECT_FALSE(Pattern::compile("x*", plain())->match_prefix(path, false));
}

TEST(PatternMatching, SuffixShortestAndLongest) {
  auto pattern = Pattern::compile(".*", plain());
  ASSERT_TRUE(pattern);
  strv name = "libfoo.tar.gz";
  auto shortest = pattern->match_suffix(name, false);
  ASSERT_TRUE(shortest);
  EXPECT_EQ(shortest->span, (MatchSpan{10, 13}));
  auto longest = pattern->match_suffix(name, true);
  ASSERT_TRUE(longest);
  EXPECT_EQ(longest->span, (MatchSpan{6, 13}));
}

TEST(PatternMatching, SearchFindsLeftmost) {
  auto pattern = Pattern::compile("l*b", plain());
  ASSERT_TRUE(pattern);
  strv text = "/usr/local/lib/libfoo";
  auto longest = pattern->search(text, 0, true);
  ASSERT_TRUE(longest);
  EXPECT_EQ(longest->span.start, 5u);
  EXPECT_EQ(longest->span.end, 18u);
  auto shortest = pattern->search(text, 0, false);
  ASSERT_TRUE(shortest);
  EXPECT_EQ(shortest->span, (MatchSpan{5, 14}));
  auto later = pattern->search(text, 15, false);
  ASSERT_TRUE(later);
  EXPECT_EQ(later->span.start, 15u);
}

TEST(HasGlobChars, DetectsUnquotedSpecials) {
  EXPECT_TRUE(has_glob_chars(QString("*.c"), plain()));
  EXPECT_TRUE(has_glob_chars(QString("a?"), plain()));
  EXPECT_TRUE(has_glob_chars(QString("[ab]"), plain()));
  EXPECT_FALSE(has_glob_chars(QString("plain"), plain()));
  EXPECT_FALSE(has_glob_chars(QString::literal("*.c"), plain()));
  EXPECT_FALSE(has_glob_chars(QString("^x"), plain()));
  EXPECT_TRUE(has_glob_chars(QString("^x"), extended()));
}

}  // namespace
}  // namespace myzsh
