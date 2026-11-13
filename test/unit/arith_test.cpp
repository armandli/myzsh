#include <gtest/gtest.h>

#include <arith/arith.h>
#include <support/test_support.h>
#include <util/error.h>

namespace myzsh {
namespace {

struct ArithTest : ::testing::Test {
  uptr<Shell> shell = test::make_shell();
  ArithEvaluator arith{*shell};

  s::int64_t integer(strv expression) {
    MathResult result = arith.evaluate(expression);
    EXPECT_FALSE(result.number.is_float()) << expression;
    return result.number.as_integer();
  }
  double real(strv expression) {
    MathResult result = arith.evaluate(expression);
    EXPECT_TRUE(result.number.is_float()) << expression;
    return result.number.as_float();
  }
  str text(strv expression) { return arith.evaluate_to_string(expression); }
};

TEST(MathNumber, Conversions) {
  EXPECT_EQ(MathNumber(s::int64_t{7}).as_float(), 7.0);
  EXPECT_EQ(MathNumber(2.9).as_integer(), 2);
  EXPECT_EQ(MathNumber(-2.9).as_integer(), -2);
  EXPECT_TRUE(MathNumber(1.0).is_float());
  EXPECT_FALSE(MathNumber(s::int64_t{1}).is_float());
}

TEST_F(ArithTest, IntegerOperators) {
  EXPECT_EQ(integer("1 + 2 * 3"), 7);
  EXPECT_EQ(integer("(1 + 2) * 3"), 9);
  EXPECT_EQ(integer("7 / 2"), 3);
  EXPECT_EQ(integer("-7 / 2"), -3);
  EXPECT_EQ(integer("-7 % 3"), -1);
  EXPECT_EQ(integer("10 % 3"), 1);
  EXPECT_EQ(integer("1 << 4"), 16);
  EXPECT_EQ(integer("256 >> 2"), 64);
  EXPECT_EQ(integer("6 & 3"), 2);
  EXPECT_EQ(integer("6 | 3"), 7);
  EXPECT_EQ(integer("5 ^ 3"), 6);
  EXPECT_EQ(integer("~0"), -1);
  EXPECT_EQ(integer("!5"), 0);
}

TEST_F(ArithTest, Power) {
  EXPECT_EQ(integer("2 ** 10"), 1024);
  // Right associative.
  EXPECT_EQ(integer("2 ** 3 ** 2"), 512);
  // Unary minus binds tighter than ** in zsh.
  EXPECT_EQ(integer("-2 ** 2"), 4);
}

TEST_F(ArithTest, ComparisonAndLogic) {
  EXPECT_EQ(integer("3 > 2"), 1);
  EXPECT_EQ(integer("3 <= 2"), 0);
  EXPECT_EQ(integer("2 == 2 && 3 != 4"), 1);
  EXPECT_EQ(integer("3 > 2 && 0 || 4"), 1);
  EXPECT_EQ(integer("1 ^^ 1"), 0);
  EXPECT_EQ(integer("1 ^^ 0"), 1);
}

TEST_F(ArithTest, TernaryAndComma) {
  EXPECT_EQ(integer("5 > 3 ? 10 : 20"), 10);
  EXPECT_EQ(integer("0 ? 10 : 20"), 20);
  EXPECT_EQ(integer("1, 2"), 2);
}

TEST_F(ArithTest, Bases) {
  EXPECT_EQ(integer("16#ff"), 255);
  EXPECT_EQ(integer("0x1f"), 31);
  EXPECT_EQ(integer("2#101"), 5);
  EXPECT_EQ(integer("0b101"), 5);
  EXPECT_EQ(integer("36#z"), 35);
  // Leading zeros are decimal unless OCTAL_ZEROES.
  EXPECT_EQ(integer("010"), 10);
  shell->options.set(Option::Octalzeroes, true);
  EXPECT_EQ(integer("010"), 8);
}

TEST_F(ArithTest, InvalidBase) {
  EXPECT_THROW(arith.evaluate("1#10"), ShellError);
}

TEST_F(ArithTest, CharacterCodes) {
  EXPECT_EQ(integer("##a"), 97);
  shell->params.set_scalar("c", "A");
  EXPECT_EQ(integer("#c"), 65);
}

TEST_F(ArithTest, Floats) {
  EXPECT_DOUBLE_EQ(real("7 / 2.0"), 3.5);
  EXPECT_DOUBLE_EQ(real("5.0 / 2"), 2.5);
  EXPECT_DOUBLE_EQ(real("1e3"), 1000.0);
  EXPECT_DOUBLE_EQ(real("1.5e1"), 15.0);
  EXPECT_DOUBLE_EQ(real("3 / 2 * 2.0"), 2.0);
}

TEST_F(ArithTest, FloatOutputFormat) {
  EXPECT_EQ(text("3.0"), "3.");
  EXPECT_EQ(text("1 / 3.0"), "0.33333333333333331");
  EXPECT_EQ(text("0.1 + 0.2"), "0.30000000000000004");
  EXPECT_EQ(text("1 / 0.0"), "Inf");
  EXPECT_EQ(text("2 ** 0.5"), "1.4142135623730951");
}

TEST_F(ArithTest, OutputBase) {
  EXPECT_EQ(text("[#16] 255"), "16#FF");
  EXPECT_EQ(text("[##16] 255"), "FF");
  EXPECT_EQ(text("[#2] 5"), "2#101");
  MathResult result = arith.evaluate("[#8] 8");
  EXPECT_EQ(result.output_base, 8);
  EXPECT_FALSE(result.suppress_base_prefix);
}

TEST_F(ArithTest, IntegerOverflowWraps) {
  EXPECT_EQ(integer("9223372036854775807 + 1"),
            s::numeric_limits<s::int64_t>::min());
}

TEST_F(ArithTest, Variables) {
  shell->params.set_scalar("x", "5");
  EXPECT_EQ(integer("x * 2"), 10);
  EXPECT_EQ(integer("$x * 2"), 10);
  // Unset is zero.
  EXPECT_EQ(integer("unset_variable + 1"), 1);
}

TEST_F(ArithTest, VariableValueIsAnExpression) {
  shell->params.set_scalar("x", "1+2");
  EXPECT_EQ(integer("x * 2"), 6);
}

TEST_F(ArithTest, Assignment) {
  EXPECT_EQ(integer("a = 3, a *= 2, a"), 6);
  EXPECT_EQ(shell->params.get_scalar("a"), "6");
  EXPECT_EQ(integer("a += 4"), 10);
  EXPECT_EQ(integer("a -= 1"), 9);
  EXPECT_EQ(integer("a /= 2"), 4);
  EXPECT_EQ(integer("a %= 3"), 1);
  EXPECT_EQ(integer("a <<= 3"), 8);
  EXPECT_EQ(integer("a **= 2"), 64);
  EXPECT_EQ(integer("a |= 1"), 65);
  EXPECT_EQ(integer("a &&= 0"), 0);
}

TEST_F(ArithTest, IncrementDecrement) {
  shell->params.set_integer("i", 5);
  EXPECT_EQ(integer("i++ + ++i"), 12);
  EXPECT_EQ(shell->params.get_integer("i"), 7);
  EXPECT_EQ(integer("i--"), 7);
  EXPECT_EQ(integer("--i"), 5);
}

TEST_F(ArithTest, ShortCircuitSkipsSideEffects) {
  shell->params.set_integer("n", 0);
  integer("0 && (n = 1)");
  integer("1 || (n = 2)");
  integer("1 ? 0 : (n = 3)");
  EXPECT_EQ(shell->params.get_integer("n"), 0);
}

TEST_F(ArithTest, ArrayElements) {
  shell->params.set_array("arr", {"10", "20", "30"});
  EXPECT_EQ(integer("arr[2] + arr[-1]"), 50);
  integer("arr[1] = 7");
  EXPECT_EQ(shell->params.get_array("arr"),
            (vec<str>{"7", "20", "30"}));
}

TEST_F(ArithTest, Errors) {
  try {
    arith.evaluate("1 / 0");
    FAIL();
  } catch (const ShellError& error) {
    EXPECT_STREQ(error.what(), "division by zero");
  }
  try {
    arith.evaluate("1 + ");
    FAIL();
  } catch (const ShellError& error) {
    EXPECT_STREQ(error.what(),
                 "bad math expression: operand expected at end of string");
  }
  try {
    arith.evaluate("x y");
    FAIL();
  } catch (const ShellError& error) {
    EXPECT_STREQ(error.what(), "bad math expression: operator expected at `y'");
  }
  EXPECT_THROW(arith.evaluate("(1 + 2"), ShellError);
  EXPECT_THROW(arith.evaluate("3 = 4"), ShellError);
}

TEST_F(ArithTest, ForceFloat) {
  shell->options.set(Option::Forcefloat, true);
  EXPECT_EQ(text("7 / 2"), "3.5");
}

TEST_F(ArithTest, EmptyExpressionIsZero) {
  EXPECT_EQ(integer(""), 0);
  EXPECT_EQ(integer("   "), 0);
}

}  // namespace
}  // namespace myzsh
