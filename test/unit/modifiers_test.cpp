#include <gtest/gtest.h>

#include <expand/modifiers.h>

namespace myzsh {
namespace {

str modify(strv value, strv chain) {
  auto parsed = parse_modifiers(chain);
  EXPECT_TRUE(parsed) << chain;
  ModifierContext context;
  context.pwd = "/home/me/src";
  return parsed ? apply_modifiers(value, *parsed, context) : str();
}

constexpr strv PATH_VALUE = "/usr/local/lib/libfoo.tar.gz";

TEST(ParseModifiers, Chain) {
  auto parsed = parse_modifiers(":h:t");
  ASSERT_TRUE(parsed);
  ASSERT_EQ(parsed->size(), 2u);
  EXPECT_EQ((*parsed)[0].op, 'h');
  EXPECT_EQ((*parsed)[1].op, 't');
}

TEST(ParseModifiers, SubstitutionAndCounts) {
  auto parsed = parse_modifiers(":gs/a/b/:h2");
  ASSERT_TRUE(parsed);
  ASSERT_EQ(parsed->size(), 2u);
  EXPECT_EQ((*parsed)[0].op, 's');
  EXPECT_TRUE((*parsed)[0].global);
  EXPECT_EQ((*parsed)[0].left, "a");
  EXPECT_EQ((*parsed)[0].right, "b");
  EXPECT_EQ((*parsed)[1].count, 2);
}

TEST(ParseModifiers, AnyDelimiter) {
  auto parsed = parse_modifiers(":s,x,y");
  ASSERT_TRUE(parsed);
  EXPECT_EQ((*parsed)[0].left, "x");
  EXPECT_EQ((*parsed)[0].right, "y");
}

TEST(ParseModifiers, UnknownModifier) {
  EXPECT_FALSE(parse_modifiers(":k"));
}

// Expected values from zsh 5.9 parameter modifiers.
TEST(ApplyModifiers, PathComponents) {
  EXPECT_EQ(modify(PATH_VALUE, ":h"), "/usr/local/lib");
  EXPECT_EQ(modify(PATH_VALUE, ":t"), "libfoo.tar.gz");
  EXPECT_EQ(modify(PATH_VALUE, ":r"), "/usr/local/lib/libfoo.tar");
  EXPECT_EQ(modify(PATH_VALUE, ":e"), "gz");
  EXPECT_EQ(modify(PATH_VALUE, ":t:r"), "libfoo.tar");
  EXPECT_EQ(modify(PATH_VALUE, ":h:h"), "/usr/local");
  EXPECT_EQ(modify(PATH_VALUE, ":h2"), "/usr");
  EXPECT_EQ(modify(PATH_VALUE, ":t2"), "lib/libfoo.tar.gz");
}

TEST(ApplyModifiers, EdgeCases) {
  EXPECT_EQ(modify("file", ":h"), ".");
  EXPECT_EQ(modify("/", ":h"), "/");
  EXPECT_EQ(modify("/a", ":h"), "/");
  EXPECT_EQ(modify("noext", ":e"), "");
  EXPECT_EQ(modify("dir.d/file", ":r"), "dir.d/file");
  EXPECT_EQ(modify("a/b/", ":t"), "b");
}

TEST(ApplyModifiers, Case) {
  EXPECT_EQ(modify(PATH_VALUE, ":u"), "/USR/LOCAL/LIB/LIBFOO.TAR.GZ");
  EXPECT_EQ(modify("MiXeD", ":l"), "mixed");
}

TEST(ApplyModifiers, Substitution) {
  EXPECT_EQ(modify("libfoo.tar.gz", ":s/foo/bar/"), "libbar.tar.gz");
  EXPECT_EQ(modify(PATH_VALUE, ":gs/l/L/"),
            "/usr/LocaL/Lib/Libfoo.tar.gz");
  EXPECT_EQ(modify("aaa", ":s/a/&&/"), "aaaa");
  EXPECT_EQ(modify("abc", ":s/x/y/"), "abc");
}

TEST(ApplyModifiers, RepeatSubstitutionRemembersLast) {
  auto parsed = parse_modifiers(":s/a/b/:&");
  ASSERT_TRUE(parsed);
  ModifierContext context;
  EXPECT_EQ(apply_modifiers("aaa", *parsed, context), "bba");
}

TEST(ApplyModifiers, Quoting) {
  EXPECT_EQ(modify("a b", ":q"), "a\\ b");
  EXPECT_EQ(modify("'a b'", ":Q"), "a b");
  // :x is the history form of :q that also splits at blanks.
  EXPECT_EQ(modify("a b c", ":x"), "'a' 'b' 'c'");
}

TEST(ApplyModifiers, AbsolutePath) {
  EXPECT_EQ(modify("x/../y/./z", ":a"), "/home/me/src/y/z");
  EXPECT_EQ(modify("/a/b/../c", ":a"), "/a/c");
}

TEST(ApplyModifiers, CommandPath) {
  auto parsed = parse_modifiers(":c");
  ASSERT_TRUE(parsed);
  ModifierContext context;
  context.path = {"/bin", "/usr/bin"};
  EXPECT_EQ(apply_modifiers("sh", *parsed, context), "/bin/sh");
  EXPECT_EQ(apply_modifiers("no_such_cmd_xyz", *parsed, context),
            "no_such_cmd_xyz");
}

}  // namespace
}  // namespace myzsh
