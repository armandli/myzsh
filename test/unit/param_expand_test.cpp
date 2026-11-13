#include <gtest/gtest.h>

#include <expand/param_expand.h>
#include <support/test_support.h>
#include <util/error.h>
#include <util/strings.h>

namespace myzsh {
namespace {

ParamExpr parsed(strv inner) {
  auto result = parse_param_expr(inner);
  EXPECT_TRUE(result) << inner;
  return result ? *result : ParamExpr{};
}

TEST(ParseParamExpr, PlainName) {
  ParamExpr expr = parsed("PATH");
  EXPECT_EQ(expr.name, "PATH");
  EXPECT_EQ(expr.op, ParamOp::None);
  EXPECT_FALSE(expr.nested);
}

TEST(ParseParamExpr, LengthAndSetTest) {
  EXPECT_TRUE(parsed("#x").length);
  EXPECT_EQ(parsed("#x").name, "x");
  EXPECT_TRUE(parsed("+x").is_set_test);
  // ${#} alone is $#.
  EXPECT_EQ(parsed("#").name, "#");
  EXPECT_FALSE(parsed("#").length);
}

TEST(ParseParamExpr, Flags) {
  ParamExpr expr = parsed("(j:,:qq)a");
  ASSERT_EQ(expr.flags.size(), 2u);
  EXPECT_EQ(expr.flags[0].flag, 'j');
  EXPECT_EQ(expr.flags[0].arguments, vec<str>{","});
  EXPECT_EQ(expr.flags[1].flag, 'q');
  EXPECT_EQ(expr.flags[1].repeat, 2);
  ParamExpr pad = parsed("(l:5::0:)x");
  EXPECT_EQ(pad.flags[0].arguments, (vec<str>{"5", "0"}));
}

TEST(ParseParamExpr, SplitGlobRcPrefixes) {
  EXPECT_TRUE(parsed("=x").split);
  EXPECT_TRUE(parsed("==x").no_split);
  EXPECT_TRUE(parsed("~x").glob_subst);
  EXPECT_TRUE(parsed("^x").rc_expand);
}

TEST(ParseParamExpr, DefaultFamily) {
  ParamExpr expr = parsed("x:-def");
  EXPECT_EQ(expr.op, ParamOp::Default);
  EXPECT_TRUE(expr.colon);
  EXPECT_EQ(expr.operand, "def");
  EXPECT_FALSE(parsed("x-def").colon);
  EXPECT_EQ(parsed("x:=v").op, ParamOp::Assign);
  EXPECT_EQ(parsed("x:?msg").op, ParamOp::Error);
  EXPECT_EQ(parsed("x:+alt").op, ParamOp::Alternate);
}

TEST(ParseParamExpr, PatternOps) {
  ParamExpr shortest = parsed("p#*/");
  EXPECT_EQ(shortest.op, ParamOp::RemovePrefix);
  EXPECT_FALSE(shortest.longest);
  EXPECT_EQ(shortest.operand, "*/");
  EXPECT_TRUE(parsed("p##*/").longest);
  EXPECT_EQ(parsed("p%.*").op, ParamOp::RemoveSuffix);
  ParamExpr replace = parsed("p/lib/LIB");
  EXPECT_EQ(replace.op, ParamOp::Replace);
  EXPECT_EQ(replace.operand, "lib");
  EXPECT_EQ(replace.replacement, "LIB");
  EXPECT_EQ(parsed("p//a/b").op, ParamOp::ReplaceAll);
  EXPECT_EQ(parsed("p/#a/b").op, ParamOp::ReplaceStart);
  EXPECT_EQ(parsed("p/%a/b").op, ParamOp::ReplaceEnd);
  EXPECT_EQ(parsed("a:#t*").op, ParamOp::ArrayFilter);
}

TEST(ParseParamExpr, SubstringAndModifiers) {
  ParamExpr sub = parsed("p:5:5");
  EXPECT_EQ(sub.op, ParamOp::Substring);
  EXPECT_EQ(sub.operand, "5");
  EXPECT_EQ(sub.replacement, "5");
  EXPECT_EQ(parsed("p: -2").op, ParamOp::Substring);
  ParamExpr mods = parsed("p:h:t");
  EXPECT_EQ(mods.op, ParamOp::Modifiers);
  EXPECT_EQ(mods.modifiers.size(), 2u);
}

TEST(ParseParamExpr, ArrayOperators) {
  EXPECT_EQ(parsed("a:|b").op, ParamOp::ArrayDifference);
  EXPECT_EQ(parsed("a:*b").op, ParamOp::ArrayIntersect);
  ParamExpr zip = parsed("a:^^b");
  EXPECT_EQ(zip.op, ParamOp::ArrayZip);
  EXPECT_TRUE(zip.longest);
}

TEST(ParseParamExpr, SubscriptsAndNesting) {
  ParamExpr sub = parsed("a[2,3]");
  EXPECT_EQ(sub.name, "a");
  EXPECT_EQ(sub.subscripts, vec<str>{"2,3"});
  ParamExpr nested = parsed("${x#a}%b");
  EXPECT_TRUE(nested.nested);
  EXPECT_EQ(nested.name, "${x#a}");
  EXPECT_EQ(nested.op, ParamOp::RemoveSuffix);
  ParamExpr quoted = parsed("(f)\"$(cmd)\"");
  EXPECT_TRUE(quoted.nested);
}

TEST(ParseParamExpr, Errors) {
  EXPECT_FALSE(parse_param_expr("(j:,)a"));
  EXPECT_FALSE(parse_param_expr("a:k"));
}

struct ParamExpandTest : ::testing::Test {
  uptr<Shell> shell = test::make_shell();
  ParamExpander expander{*shell};

  void SetUp() override {
    shell->params.set_array("a", {"one", "two", "three", "four", "five"});
    shell->params.set_scalar("s", "hello world");
    shell->params.set_scalar("p", "/usr/local/lib/libfoo.tar.gz");
    shell->params.set_scalar("e", "");
    shell->params.declare("h", ParamType::Assoc, PF_NONE, false);
    shell->params.set_array("h", {"k1", "v1", "k2", "v2"});
  }

  ExpansionValue value(strv inner, bool quoted = false) {
    auto expr = parse_param_expr(inner);
    EXPECT_TRUE(expr) << inner;
    return expr ? expander.expand(*expr, quoted) : ExpansionValue{};
  }
  // Words joined by a space, the way `print` shows them.
  str text(strv inner, bool quoted = false) {
    return join(value(inner, quoted).values, " ");
  }
};

// Expected values from zsh 5.9 (see TODO.md "verifying behaviour").
TEST_F(ParamExpandTest, SimpleValues) {
  EXPECT_EQ(text("s"), "hello world");
  EXPECT_EQ(text("a"), "one two three four five");
  EXPECT_TRUE(value("a").is_array);
  EXPECT_FALSE(value("s").is_array);
  EXPECT_EQ(value("unset_name").values, vec<str>{});
}

TEST_F(ParamExpandTest, Lengths) {
  EXPECT_EQ(text("#a"), "5");
  EXPECT_EQ(text("#s"), "11");
  EXPECT_EQ(text("#a[1]"), "3");
  EXPECT_EQ(text("(c)#a"), "23");
}

TEST_F(ParamExpandTest, Subscripts) {
  EXPECT_EQ(text("a[2]"), "two");
  EXPECT_EQ(text("a[-1]"), "five");
  EXPECT_EQ(text("a[2,-2]"), "two three four");
  EXPECT_EQ(text("s[2,4]"), "ell");
  EXPECT_EQ(text("h[k1]"), "v1");
}

TEST_F(ParamExpandTest, DefaultAssignAlternate) {
  EXPECT_EQ(text("u:-def"), "def");
  EXPECT_EQ(text("u-def2"), "def2");
  EXPECT_EQ(text("+u"), "0");
  EXPECT_EQ(text("u:+alt"), "");
  EXPECT_EQ(text("e:-d1"), "d1");
  EXPECT_EQ(text("e-d2"), "");
  EXPECT_EQ(text("e:+alt"), "");
  EXPECT_EQ(text("s:+alt"), "alt");
  EXPECT_EQ(text("u:=assigned"), "assigned");
  EXPECT_EQ(shell->params.get_scalar("u"), "assigned");
  EXPECT_EQ(text("+u"), "1");
}

TEST_F(ParamExpandTest, ErrorOperator) {
  try {
    value("u:?custom message");
    FAIL();
  } catch (const ShellError& error) {
    EXPECT_STREQ(error.what(), "u: custom message");
  }
  try {
    value("u:?");
    FAIL();
  } catch (const ShellError& error) {
    EXPECT_STREQ(error.what(), "u: parameter not set");
  }
  try {
    value("e:?");
    FAIL();
  } catch (const ShellError& error) {
    EXPECT_STREQ(error.what(), "e: parameter not set");
  }
}

TEST_F(ParamExpandTest, NoUnsetOption) {
  shell->options.set(Option::Unset, false);
  try {
    value("never_set");
    FAIL();
  } catch (const ShellError& error) {
    EXPECT_STREQ(error.what(), "never_set: parameter not set");
  }
}

TEST_F(ParamExpandTest, PrefixSuffixRemoval) {
  EXPECT_EQ(text("p#*/"), "usr/local/lib/libfoo.tar.gz");
  EXPECT_EQ(text("p##*/"), "libfoo.tar.gz");
  EXPECT_EQ(text("p%.*"), "/usr/local/lib/libfoo.tar");
  EXPECT_EQ(text("p%%.*"), "/usr/local/lib/libfoo");
}

TEST_F(ParamExpandTest, Replacement) {
  EXPECT_EQ(text("p/lib/LIB"), "/usr/local/LIB/libfoo.tar.gz");
  EXPECT_EQ(text("p//lib/LIB"), "/usr/local/LIB/LIBfoo.tar.gz");
  EXPECT_EQ(text("p/#\\/usr/X"), "X/local/lib/libfoo.tar.gz");
  EXPECT_EQ(text("p/%gz/bz2"), "/usr/local/lib/libfoo.tar.bz2");
  EXPECT_EQ(text("a/#t/T"), "one Two Three four five");
  EXPECT_EQ(text("a/%e/E"), "onE two threE four fivE");
}

TEST_F(ParamExpandTest, Substring) {
  EXPECT_EQ(text("p:5"), "local/lib/libfoo.tar.gz");
  EXPECT_EQ(text("p:5:5"), "local");
  EXPECT_EQ(text("p: -2"), "gz");
  EXPECT_EQ(text("a:1:2"), "two three");
}

TEST_F(ParamExpandTest, Modifiers) {
  EXPECT_EQ(text("p:h"), "/usr/local/lib");
  EXPECT_EQ(text("p:t:r"), "libfoo.tar");
  EXPECT_EQ(text("p:u"), "/USR/LOCAL/LIB/LIBFOO.TAR.GZ");
}

TEST_F(ParamExpandTest, ArrayFilters) {
  EXPECT_EQ(text("a:#t*"), "one four five");
  EXPECT_EQ(text("(M)a:#t*"), "two three");
  shell->params.set_array("b", {"three", "six"});
  EXPECT_EQ(text("a:|b"), "one two four five");
  EXPECT_EQ(text("a:*b"), "three");
  EXPECT_EQ(text("a:^b"), "one three two six");
}

TEST_F(ParamExpandTest, JoinAndSplitFlags) {
  EXPECT_EQ(text("(j:,:)a"), "one,two,three,four,five");
  shell->params.set_scalar("csv", "x,y,,z");
  EXPECT_EQ(value("(s:,:)csv").values, (vec<str>{"x", "y", "z"}));
  EXPECT_EQ(value("(@s:,:)csv", true).values,
            (vec<str>{"x", "y", "", "z"}));
  shell->params.set_scalar("lines", "l1\nl2\n");
  EXPECT_EQ(value("(f)lines").values, (vec<str>{"l1", "l2"}));
  shell->params.set_scalar("cmd", "echo 'a b' | wc");
  EXPECT_EQ(value("(z)cmd").values, (vec<str>{"echo", "'a b'", "|", "wc"}));
  shell->params.set_scalar("nul", str("a\0b", 3));
  EXPECT_EQ(value("(0)nul").values, (vec<str>{"a", "b"}));
}

TEST_F(ParamExpandTest, Sorting) {
  EXPECT_EQ(text("(o)a"), "five four one three two");
  EXPECT_EQ(text("(O)a"), "two three one four five");
  shell->params.set_array("n", {"10", "9", "100"});
  EXPECT_EQ(text("(on)n"), "9 10 100");
  shell->params.set_array("c", {"b", "A", "a", "B"});
  EXPECT_EQ(text("(oi)c").substr(0, 3), "A a");
  EXPECT_EQ(text("(Oa)a"), "five four three two one");
}

TEST_F(ParamExpandTest, UniqueAndCase) {
  shell->params.set_array("d", {"a", "a", "b"});
  EXPECT_EQ(text("(u)d"), "a b");
  EXPECT_EQ(text("(U)s"), "HELLO WORLD");
  EXPECT_EQ(text("(L)a[1]"), "one");
  EXPECT_EQ(text("(C)s"), "Hello World");
}

TEST_F(ParamExpandTest, Padding) {
  shell->params.set_scalar("num", "42");
  EXPECT_EQ(text("(l:5::0:)num"), "00042");
  shell->params.set_scalar("ab", "ab");
  EXPECT_EQ(text("(r:5::.:)ab"), "ab...");
  EXPECT_EQ(text("(l:1:)ab"), "b");
}

TEST_F(ParamExpandTest, Quoting) {
  shell->params.set_scalar("q", "it's");
  EXPECT_EQ(text("(q)q"), "it\\'s");
  EXPECT_EQ(text("(qq)q"), "'it'\\''s'");
  shell->params.set_scalar("quoted", "'a b'");
  EXPECT_EQ(text("(Q)quoted"), "a b");
}

TEST_F(ParamExpandTest, KeysValuesAndType) {
  EXPECT_EQ(text("(o)${(k)h}"), "k1 k2");
  EXPECT_EQ(text("(kv)#h"), "4");
  EXPECT_EQ(text("(t)a"), "array");
  EXPECT_EQ(text("(t)h"), "association");
  EXPECT_EQ(text("(t)s"), "scalar");
  shell->params.declare("n", ParamType::Integer, PF_NONE, false);
  EXPECT_EQ(text("(t)n"), "integer");
  shell->params.set_flags("s", PF_EXPORT, 0);
  EXPECT_EQ(text("(t)s"), "scalar-export");
}

TEST_F(ParamExpandTest, IndirectionAndPromptFlags) {
  shell->params.set_scalar("ref", "s");
  EXPECT_EQ(text("(P)ref"), "hello world");
  EXPECT_EQ(text("(%):-%%"), "%");
}

TEST_F(ParamExpandTest, EvalFlag) {
  shell->params.set_scalar("tmpl", "$s!");
  EXPECT_EQ(text("(e)tmpl"), "hello world!");
}

TEST_F(ParamExpandTest, VisibleFlag) {
  shell->params.set_scalar("ctl", "a\tb");
  EXPECT_EQ(text("(V)ctl"), "a\\tb");
}

TEST_F(ParamExpandTest, Nested) {
  EXPECT_EQ(text("${p:t}:s/foo/bar/"), "libbar.tar.gz");
  EXPECT_EQ(text("#${(s:,:)${:-x,y,,z}}"), "3");
  EXPECT_EQ(text(":-literal"), "literal");
}

TEST_F(ParamExpandTest, SplitAndRcRequestsArePropagated) {
  EXPECT_TRUE(value("=s").split);
  EXPECT_TRUE(value("~s").glob_subst);
  EXPECT_TRUE(value("^a").rc_expand);
}

TEST_F(ParamExpandTest, QuotedArrayJoinsUnlessAt) {
  EXPECT_EQ(value("a", true).values,
            vec<str>{"one two three four five"});
  EXPECT_EQ(value("a[@]", true).values.size(), 5u);
  EXPECT_EQ(value("(@)a", true).values.size(), 5u);
  EXPECT_EQ(value("a[2,3]", true).values, vec<str>{"two three"});
}

TEST_F(ParamExpandTest, QuotedStarUsesFirstIfsChar) {
  shell->params.set_scalar("IFS", ":");
  EXPECT_EQ(value("a[*]", true).values,
            vec<str>{"one:two:three:four:five"});
}

}  // namespace
}  // namespace myzsh
