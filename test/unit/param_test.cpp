#include <gtest/gtest.h>

#include <algorithm>

#include <params/param.h>
#include <support/test_support.h>
#include <util/error.h>

namespace myzsh {
namespace {

using Words = vec<str>;

struct ParamTableTest : ::testing::Test {
  uptr<Shell> shell = test::make_shell();
  ParamTable& params = shell->params;
};

TEST_F(ParamTableTest, ScalarRoundTrip) {
  params.set_scalar("x", "hello");
  EXPECT_TRUE(params.is_set("x"));
  EXPECT_EQ(params.get_scalar("x"), "hello");
  EXPECT_EQ(params.find("x")->get().type, ParamType::Scalar);
}

TEST_F(ParamTableTest, UnsetParameter) {
  EXPECT_FALSE(params.is_set("nope"));
  EXPECT_EQ(params.get_scalar("nope"), s::nullopt);
  params.set_scalar("x", "1");
  EXPECT_TRUE(params.unset("x"));
  EXPECT_FALSE(params.is_set("x"));
  EXPECT_FALSE(params.unset("x"));
}

TEST_F(ParamTableTest, ArrayRoundTripAndScalarView) {
  params.set_array("a", {"one", "two", "three"});
  EXPECT_EQ(params.get_array("a"), (Words{"one", "two", "three"}));
  EXPECT_EQ(params.get_scalar("a"), "one two three");
  EXPECT_EQ(params.find("a")->get().type, ParamType::Array);
}

TEST_F(ParamTableTest, ScalarAsArrayIsOneElement) {
  params.set_scalar("s", "a b");
  EXPECT_EQ(params.get_array("s"), Words{"a b"});
}

TEST_F(ParamTableTest, AssignScalarToArrayReplacesIt) {
  params.set_array("a", {"x", "y"});
  params.set_scalar("a", "z");
  EXPECT_EQ(params.find("a")->get().type, ParamType::Scalar);
  EXPECT_EQ(params.get_scalar("a"), "z");
}

TEST_F(ParamTableTest, AssocRoundTrip) {
  params.declare("h", ParamType::Assoc, PF_NONE, false);
  params.set_array("h", {"k1", "v1", "k2", "v2"});
  Assoc expected{{"k1", "v1"}, {"k2", "v2"}};
  EXPECT_EQ(params.get_assoc("h"), expected);
}

TEST_F(ParamTableTest, AssocOddCountIsError) {
  params.declare("h", ParamType::Assoc, PF_NONE, false);
  EXPECT_THROW(params.set_array("h", {"k1", "v1", "k2"}), ShellError);
}

TEST_F(ParamTableTest, IntegerEvaluatesArithmetic) {
  params.declare("n", ParamType::Integer, PF_NONE, false);
  params.set_scalar("n", "3 + 4");
  EXPECT_EQ(params.get_integer("n"), 7);
  EXPECT_EQ(params.get_scalar("n"), "7");
}

TEST_F(ParamTableTest, IntegerBaseFormatting) {
  Param& param = params.declare("n", ParamType::Integer, PF_NONE, false);
  param.base = 16;
  params.set_integer("n", 255);
  EXPECT_EQ(params.get_scalar("n"), "16#FF");
}

TEST_F(ParamTableTest, FloatFormatting) {
  params.declare("f", ParamType::Float, PF_EXPONENT, false);
  params.set_float("f", 1.5);
  EXPECT_EQ(params.get_scalar("f"), "1.500000000e+00");
}

TEST_F(ParamTableTest, ReadonlyRejectsAssignmentAndUnset) {
  params.set_scalar("r", "1");
  params.set_flags("r", PF_READONLY, 0);
  try {
    params.set_scalar("r", "2");
    FAIL() << "expected ShellError";
  } catch (const ShellError& error) {
    EXPECT_STREQ(error.what(), "read-only variable: r");
  }
  EXPECT_THROW(params.unset("r"), ShellError);
  EXPECT_EQ(params.get_scalar("r"), "1");
}

TEST_F(ParamTableTest, CaseConversionFlags) {
  params.declare("lo", ParamType::Scalar, PF_LOWER, false);
  params.set_scalar("lo", "MiXeD");
  EXPECT_EQ(params.get_scalar("lo"), "mixed");
  params.declare("up", ParamType::Scalar, PF_UPPER, false);
  params.set_scalar("up", "MiXeD");
  EXPECT_EQ(params.get_scalar("up"), "MIXED");
}

TEST_F(ParamTableTest, JustificationFlags) {
  Param& left = params.declare("l", ParamType::Scalar, PF_LEFT, false);
  left.width = 5;
  params.set_scalar("l", "ab");
  EXPECT_EQ(params.get_scalar("l"), "ab   ");
  Param& zero = params.declare("z", ParamType::Scalar, PF_RIGHT_ZERO, false);
  zero.width = 4;
  params.set_scalar("z", "42");
  EXPECT_EQ(params.get_scalar("z"), "0042");
  Param& right = params.declare("r", ParamType::Scalar, PF_RIGHT_BLANK, false);
  right.width = 3;
  params.set_scalar("r", "abcdef");
  EXPECT_EQ(params.get_scalar("r"), "def");
}

TEST_F(ParamTableTest, UniqueArray) {
  params.declare("u", ParamType::Array, PF_UNIQUE, false);
  params.set_array("u", {"a", "b", "a", "c", "b"});
  EXPECT_EQ(params.get_array("u"), (Words{"a", "b", "c"}));
}

TEST_F(ParamTableTest, AppendOperations) {
  params.set_scalar("s", "ab");
  params.append_scalar("s", "cd");
  EXPECT_EQ(params.get_scalar("s"), "abcd");
  params.set_array("a", {"x"});
  params.append_array("a", {"y", "z"});
  EXPECT_EQ(params.get_array("a"), (Words{"x", "y", "z"}));
  params.declare("n", ParamType::Integer, PF_NONE, false);
  params.set_integer("n", 5);
  params.append_scalar("n", "3");
  EXPECT_EQ(params.get_integer("n"), 8);
}

TEST_F(ParamTableTest, LocalScopeShadowsAndRestores) {
  params.set_scalar("x", "global");
  params.push_scope();
  EXPECT_EQ(params.scope_level(), 1);
  params.declare("x", ParamType::Scalar, PF_NONE, true);
  params.set_scalar("x", "local");
  EXPECT_EQ(params.get_scalar("x"), "local");
  params.pop_scope();
  EXPECT_EQ(params.scope_level(), 0);
  EXPECT_EQ(params.get_scalar("x"), "global");
}

TEST_F(ParamTableTest, LocalWithoutValueIsEmptyNotOuter) {
  params.set_scalar("x", "outer");
  params.push_scope();
  params.declare("x", ParamType::Scalar, PF_NONE, true);
  EXPECT_EQ(params.get_scalar("x"), "");
  params.pop_scope();
}

TEST_F(ParamTableTest, UnsetLocalDoesNotRevealOuter) {
  params.set_scalar("x", "outer");
  params.push_scope();
  params.declare("x", ParamType::Scalar, PF_NONE, true);
  params.set_scalar("x", "inner");
  params.unset("x");
  EXPECT_FALSE(params.is_set("x"));
  params.pop_scope();
  EXPECT_EQ(params.get_scalar("x"), "outer");
}

TEST_F(ParamTableTest, AssignmentInFunctionWithoutLocalIsGlobal) {
  params.push_scope();
  params.set_scalar("g", "set inside");
  params.pop_scope();
  EXPECT_EQ(params.get_scalar("g"), "set inside");
}

TEST_F(ParamTableTest, ExportedEnvironment) {
  params.set_scalar("VISIBLE", "1");
  params.set_flags("VISIBLE", PF_EXPORT, 0);
  params.set_scalar("HIDDEN", "2");
  Words env = params.environment();
  EXPECT_NE(s::ranges::find(env, "VISIBLE=1"), env.end());
  EXPECT_EQ(s::ranges::find(env, "HIDDEN=2"), env.end());
}

TEST_F(ParamTableTest, ExportedArrayJoinsWithColonForTied) {
  params.set_array("path", {"/bin", "/usr/bin"});
  EXPECT_EQ(params.get_scalar("PATH"), "/bin:/usr/bin");
  params.set_scalar("PATH", "/a:/b:/c");
  EXPECT_EQ(params.get_array("path"), (Words{"/a", "/b", "/c"}));
}

TEST_F(ParamTableTest, ImportEnvironment) {
  char a[] = "FOO=bar";
  char b[] = "NOT-AN-IDENT=x";
  char c[] = "EMPTY=";
  char* envp[] = {a, b, c, nullptr};
  params.import_environment(envp);
  EXPECT_EQ(params.get_scalar("FOO"), "bar");
  EXPECT_EQ(params.get_scalar("EMPTY"), "");
  EXPECT_TRUE(params.find("FOO")->get().has(PF_EXPORT));
  EXPECT_FALSE(params.is_set("NOT-AN-IDENT"));
}

TEST_F(ParamTableTest, SpecialParameterHooks) {
  int counter = 0;
  SpecialParam special;
  special.type = ParamType::Integer;
  special.get = [&] { return ParamValue(s::int64_t{++counter}); };
  special.set = [&](const ParamValue& value) {
    counter = static_cast<int>(s::get<s::int64_t>(value));
  };
  params.add_special("COUNTER", special, PF_SPECIAL);
  EXPECT_EQ(params.get_scalar("COUNTER"), "1");
  EXPECT_EQ(params.get_scalar("COUNTER"), "2");
  params.set_integer("COUNTER", 10);
  EXPECT_EQ(params.get_scalar("COUNTER"), "11");
}

TEST_F(ParamTableTest, NamesAreSorted) {
  params.set_scalar("zz_b", "1");
  params.set_scalar("zz_a", "1");
  Words names = params.names();
  EXPECT_TRUE(s::ranges::is_sorted(names));
  EXPECT_NE(s::ranges::find(names, "zz_a"), names.end());
}

TEST(FormatNumbers, Floats) {
  EXPECT_EQ(format_float(3.0), "3.");
  EXPECT_EQ(format_float(1.0 / 3.0), "0.33333333333333331");
  EXPECT_EQ(format_float(1000.0), "1000.");
  EXPECT_EQ(format_float(1.5, true, 10), "1.500000000e+00");
  EXPECT_EQ(format_float(1.5, false, 3), "1.500");
}

TEST(FormatNumbers, Integers) {
  EXPECT_EQ(format_integer(255, 10), "255");
  EXPECT_EQ(format_integer(255, 16), "16#FF");
  EXPECT_EQ(format_integer(255, 16, false), "FF");
  EXPECT_EQ(format_integer(5, 2), "2#101");
  EXPECT_EQ(format_integer(-255, 16), "-16#FF");
}

}  // namespace
}  // namespace myzsh
