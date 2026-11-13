#include <gtest/gtest.h>

#include <algorithm>

#include <params/subscript.h>
#include <support/test_support.h>

namespace myzsh {
namespace {

using Words = vec<str>;

TEST(ParseSubscript, Forms) {
  Subscript index = parse_subscript("2");
  EXPECT_EQ(index.kind, Subscript::Kind::Index);
  EXPECT_EQ(index.first, "2");
  Subscript range = parse_subscript("2,-1");
  EXPECT_EQ(range.kind, Subscript::Kind::Range);
  EXPECT_EQ(range.first, "2");
  EXPECT_EQ(range.second, "-1");
  EXPECT_EQ(parse_subscript("@").kind, Subscript::Kind::All);
  Subscript star = parse_subscript("*");
  EXPECT_EQ(star.kind, Subscript::Kind::All);
  EXPECT_TRUE(star.star);
}

TEST(ParseSubscript, Flags) {
  Subscript r = parse_subscript("(r)t*");
  EXPECT_TRUE(r.reverse);
  EXPECT_EQ(r.first, "t*");
  EXPECT_EQ(r.kind, Subscript::Kind::Search);
  EXPECT_TRUE(parse_subscript("(R)x").reverse_last);
  EXPECT_TRUE(parse_subscript("(i)x").index_first);
  EXPECT_TRUE(parse_subscript("(I)x").index_last);
  Subscript w = parse_subscript("(ws:,:)2");
  EXPECT_TRUE(w.words);
  EXPECT_EQ(w.word_separator, ",");
  EXPECT_EQ(parse_subscript("(n:2:i)x").nth, 2);
  EXPECT_TRUE(parse_subscript("(e)*").exact);
}

TEST(ParseSubscript, CommaInsideParensIsNotRange) {
  Subscript sub = parse_subscript("(i)a,b");
  EXPECT_EQ(sub.kind, Subscript::Kind::Search);
  EXPECT_EQ(sub.first, "a,b");
}

struct SubscriptEvalTest : ::testing::Test {
  uptr<Shell> shell = test::make_shell();
  SubscriptEvaluator eval{*shell};
  Words array{"one", "two", "three", "four", "five"};
};

// Values below were checked against zsh 5.9.
TEST_F(SubscriptEvalTest, ArrayIndexing) {
  EXPECT_EQ(eval.read_array(array, parse_subscript("2")), Words{"two"});
  EXPECT_EQ(eval.read_array(array, parse_subscript("-1")), Words{"five"});
  EXPECT_EQ(eval.read_array(array, parse_subscript("1+1")), Words{"two"});
  EXPECT_EQ(eval.read_array(array, parse_subscript("9")), Words{});
}

TEST_F(SubscriptEvalTest, ArrayRanges) {
  EXPECT_EQ(eval.read_array(array, parse_subscript("2,3")),
            (Words{"two", "three"}));
  EXPECT_EQ(eval.read_array(array, parse_subscript("2,-2")),
            (Words{"two", "three", "four"}));
  EXPECT_EQ(eval.read_array(array, parse_subscript("4,100")),
            (Words{"four", "five"}));
  EXPECT_EQ(eval.read_array(array, parse_subscript("3,2")), Words{});
}

TEST_F(SubscriptEvalTest, ArraySearchFlags) {
  EXPECT_EQ(eval.read_array(array, parse_subscript("(i)three")), Words{"3"});
  EXPECT_EQ(eval.read_array(array, parse_subscript("(r)t*")), Words{"two"});
  EXPECT_EQ(eval.read_array(array, parse_subscript("(R)t*")),
            Words{"three"});
  EXPECT_EQ(eval.read_array(array, parse_subscript("(I)t*")), Words{"3"});
  // No match: (i) gives one past the end, (I) gives 0.
  EXPECT_EQ(eval.read_array(array, parse_subscript("(i)zzz")), Words{"6"});
  EXPECT_EQ(eval.read_array(array, parse_subscript("(I)zzz")), Words{"0"});
  EXPECT_EQ(eval.read_array(array, parse_subscript("(n:2:r)t*")),
            Words{"three"});
}

TEST_F(SubscriptEvalTest, ScalarCharacters) {
  EXPECT_EQ(eval.read_scalar("hello world", parse_subscript("1")), "h");
  EXPECT_EQ(eval.read_scalar("hello world", parse_subscript("-1")), "d");
  EXPECT_EQ(eval.read_scalar("hello world", parse_subscript("2,4")), "ell");
  EXPECT_EQ(eval.read_scalar("hello world", parse_subscript("(w)2")),
            "world");
  EXPECT_EQ(eval.read_scalar("hello world", parse_subscript("(i)o")), "5");
  EXPECT_EQ(eval.read_scalar("hello world", parse_subscript("(I)o")), "8");
  EXPECT_EQ(eval.read_scalar("a,b,c", parse_subscript("(ws:,:)2")), "b");
}

// For assocs (r)/(R) match values, (i)/(I) match keys and return keys,
// while (k)/(K) treat the stored keys as patterns matched against the
// subscript (zsh 5.9: h=(k1 v1 "k*" star) gives ${h[(k)kzz]} = star).
TEST_F(SubscriptEvalTest, AssocAccess) {
  Assoc assoc{{"k1", "v1"}, {"k*", "star"}, {"other", "v1"}};
  EXPECT_EQ(eval.read_assoc(assoc, parse_subscript("k1")), Words{"v1"});
  EXPECT_EQ(eval.read_assoc(assoc, parse_subscript("missing")), Words{});
  EXPECT_EQ(eval.read_assoc(assoc, parse_subscript("(r)s*")),
            Words{"star"});
  EXPECT_EQ(eval.read_assoc(assoc, parse_subscript("(i)k1")), Words{"k1"});
  EXPECT_EQ(eval.read_assoc(assoc, parse_subscript("(I)o*")),
            Words{"other"});
  EXPECT_EQ(eval.read_assoc(assoc, parse_subscript("(k)kzz")),
            Words{"star"});
  Words all = eval.read_assoc(assoc, parse_subscript("(K)k1"));
  s::ranges::sort(all);
  EXPECT_EQ(all, (Words{"star", "v1"}));
}

TEST_F(SubscriptEvalTest, AssignElement) {
  Words target = array;
  eval.assign_array(target, parse_subscript("2"), {"TWO"});
  EXPECT_EQ(target[1], "TWO");
}

TEST_F(SubscriptEvalTest, AssignPastEndPads) {
  Words target{"a"};
  eval.assign_array(target, parse_subscript("4"), {"d"});
  EXPECT_EQ(target, (Words{"a", "", "", "d"}));
}

TEST_F(SubscriptEvalTest, AssignSliceReplaces) {
  Words target{"a", "b", "c", "d"};
  eval.assign_array(target, parse_subscript("2,3"), {"x", "y", "z"});
  EXPECT_EQ(target, (Words{"a", "x", "y", "z", "d"}));
}

TEST_F(SubscriptEvalTest, AssignEmptyListDeletes) {
  Words target{"a", "b", "c"};
  eval.assign_array(target, parse_subscript("-1"), {});
  EXPECT_EQ(target, (Words{"a", "b"}));
}

TEST_F(SubscriptEvalTest, AssignScalarCharacters) {
  str value = "hello";
  eval.assign_scalar(value, parse_subscript("1"), "J");
  EXPECT_EQ(value, "Jello");
  eval.assign_scalar(value, parse_subscript("2,3"), "EE");
  EXPECT_EQ(value, "JEElo");
}

TEST_F(SubscriptEvalTest, KshArraysZeroBased) {
  shell->options.set(Option::Ksharrays, true);
  EXPECT_EQ(eval.read_array(array, parse_subscript("0")), Words{"one"});
}

}  // namespace
}  // namespace myzsh
