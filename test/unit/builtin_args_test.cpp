#include <gtest/gtest.h>

#include <algorithm>

#include <builtins/builtin.h>
#include <builtins/printf_format.h>
#include <support/test_support.h>

namespace myzsh {
namespace {

TEST(ParseBuiltinArgs, FlagsAndOperands) {
  auto args = parse_builtin_args({"print", "-rn", "--", "-x", "y"}, "rnl");
  ASSERT_TRUE(args);
  EXPECT_TRUE(args->minus('r'));
  EXPECT_TRUE(args->minus('n'));
  EXPECT_FALSE(args->has('l'));
  EXPECT_EQ(args->operands, (vec<str>{"-x", "y"}));
}

TEST(ParseBuiltinArgs, StopsAtFirstNonOption) {
  auto args = parse_builtin_args({"cmd", "-r", "word", "-n"}, "rn");
  ASSERT_TRUE(args);
  EXPECT_FALSE(args->has('n'));
  EXPECT_EQ(args->operands, (vec<str>{"word", "-n"}));
}

TEST(ParseBuiltinArgs, LoneDashEndsOptions) {
  auto args = parse_builtin_args({"cmd", "-", "-r"}, "r");
  ASSERT_TRUE(args);
  EXPECT_FALSE(args->has('r'));
  EXPECT_EQ(args->operands, vec<str>{"-r"});
}

TEST(ParseBuiltinArgs, ArgumentsAttachedOrSeparate) {
  auto attached = parse_builtin_args({"print", "-u2", "x"}, "u:");
  ASSERT_TRUE(attached);
  EXPECT_EQ(attached->value('u'), "2");
  auto separate = parse_builtin_args({"print", "-u", "2", "x"}, "u:");
  ASSERT_TRUE(separate);
  EXPECT_EQ(separate->value('u'), "2");
  EXPECT_EQ(separate->operands, vec<str>{"x"});
}

TEST(ParseBuiltinArgs, OptionalAttachedArgument) {
  auto bare = parse_builtin_args({"read", "-k", "x"}, "k::");
  ASSERT_TRUE(bare);
  EXPECT_TRUE(bare->has('k'));
  EXPECT_EQ(bare->value('k'), s::nullopt);
  EXPECT_EQ(bare->operands, vec<str>{"x"});
  auto attached = parse_builtin_args({"read", "-k3"}, "k::");
  ASSERT_TRUE(attached);
  EXPECT_EQ(attached->value('k'), "3");
}

TEST(ParseBuiltinArgs, PlusForm) {
  auto args = parse_builtin_args({"typeset", "+x", "-i", "n"}, "+xi");
  ASSERT_TRUE(args);
  EXPECT_TRUE(args->plus('x'));
  EXPECT_FALSE(args->minus('x'));
  EXPECT_TRUE(args->minus('i'));
  // Without '+' in optstr, "+x" is an operand.
  auto plain = parse_builtin_args({"print", "+x"}, "x");
  ASSERT_TRUE(plain);
  EXPECT_EQ(plain->operands, vec<str>{"+x"});
}

TEST(ParseBuiltinArgs, Errors) {
  auto bad = parse_builtin_args({"print", "-Q"}, "rn");
  ASSERT_FALSE(bad);
  EXPECT_EQ(bad.error(), "bad option: -Q");
  auto missing = parse_builtin_args({"print", "-u"}, "u:");
  ASSERT_FALSE(missing);
  EXPECT_EQ(missing.error(), "argument expected: -u");
}

TEST(BuiltinTable, RegisterAllAndLookup) {
  BuiltinTable table;
  table.register_all();
  for (strv name :
       {".", ":", "[", "alias", "autoload", "bg", "bindkey", "break",
        "builtin", "cd", "command", "continue", "declare", "dirs", "disable",
        "disown", "echo", "emulate", "enable", "eval", "exec", "exit",
        "export", "false", "fc", "fg", "float", "functions", "getln",
        "getopts", "hash", "history", "integer", "jobs", "kill", "let",
        "limit", "local", "logout", "noglob", "popd", "print", "printf",
        "pushd", "pushln", "pwd", "r", "read", "readonly", "rehash",
        "return", "sched", "set", "setopt", "shift", "source", "suspend", "test",
        "times", "trap", "true", "type", "typeset", "ulimit", "umask",
        "unalias", "unfunction", "unhash", "unlimit", "unset", "unsetopt",
        "vared", "wait", "whence", "where", "which", "zle", "zmodload"}) {
    EXPECT_TRUE(table.find(name)) << name;
  }
  EXPECT_TRUE(table.find("export")->get().flags & BF_SPECIAL);
  EXPECT_TRUE(table.find("typeset")->get().flags & BF_ASSIGN);
  EXPECT_TRUE(table.find("noglob")->get().flags & BF_PREFIX);
  EXPECT_FALSE(table.find("ls"));
}

TEST(BuiltinTable, DisableHidesBuiltin) {
  BuiltinTable table;
  table.register_all();
  table.set_disabled("echo", true);
  EXPECT_FALSE(table.find("echo"));
  table.set_disabled("echo", false);
  EXPECT_TRUE(table.find("echo"));
  vec<str> names = table.names();
  EXPECT_TRUE(s::ranges::is_sorted(names));
}

opt<PrintfResult> printf_result(strv format, vec<str> args) {
  auto shell = test::make_shell();
  auto result = format_printf(format, args, *shell);
  if (not result) {
    return s::nullopt;
  }
  return *result;
}

str printf_out(strv format, vec<str> args) {
  auto result = printf_result(format, args);
  EXPECT_TRUE(result) << format;
  return result ? result->output : str();
}

// Expected values from zsh 5.9 printf.
TEST(FormatPrintf, Conversions) {
  EXPECT_EQ(printf_out("%s-%d-%5.2f-%x-%o-%c-%b-%q-%%\n",
                       {"str", "42", "3.14159", "255", "8", "char", "a\\tb",
                        "a b"}),
            "str-42- 3.14-ff-10-c-a\tb-a\\ b-%\n");
}

TEST(FormatPrintf, FormatIsReused) {
  EXPECT_EQ(printf_out("%s\n", {"a", "b", "c"}), "a\nb\nc\n");
  EXPECT_EQ(printf_out("%s=%s ", {"a", "1", "b"}), "a=1 b= ");
}

TEST(FormatPrintf, WidthsAndFlags) {
  EXPECT_EQ(printf_out("%-5s|%5s|\n", {"ab", "cd"}), "ab   |   cd|\n");
  EXPECT_EQ(printf_out("%03d %+d %e %g\n", {"7", "5", "1234.5", "0.0001"}),
            "007 +5 1.234500e+03 0.0001\n");
  EXPECT_EQ(printf_out("%*d|%.*f", {"4", "7", "1", "2.25"}), "   7|2.2");
  EXPECT_EQ(printf_out("%#x %#o %X", {"255", "8", "255"}), "0xff 010 FF");
}

TEST(FormatPrintf, PositionalArguments) {
  EXPECT_EQ(printf_out("%2$s %1$s\n", {"a", "b"}), "b a\n");
}

TEST(FormatPrintf, ArithmeticAndCharacterArguments) {
  EXPECT_EQ(printf_out("%d %d", {"1+2", "'A"}), "3 65");
}

TEST(FormatPrintf, MissingArguments) {
  EXPECT_EQ(printf_out("[%s][%d]", {}), "[][0]");
}

// Numeric arguments are arithmetic: "abc" is an unset variable (0), while
// "1x" is a math error that is reported but still prints 0.
TEST(FormatPrintf, NumericArgumentsAreArithmetic) {
  auto unset = printf_result("%d", {"abc"});
  ASSERT_TRUE(unset);
  EXPECT_EQ(unset->output, "0");
  EXPECT_FALSE(unset->had_error);
  auto bad = printf_result("%d|", {"1x"});
  ASSERT_TRUE(bad);
  EXPECT_EQ(bad->output, "0|");
  EXPECT_TRUE(bad->had_error);
}

TEST(FormatPrintf, EscapesInFormat) {
  EXPECT_EQ(printf_out("a\\tb\\n", {}), "a\tb\n");
}

TEST(FormatPrintf, PercentBStopsAtBackslashC) {
  EXPECT_EQ(printf_out("%b|%s", {"x\\cy", "z"}), "x");
}

TEST(FormatPrintf, InvalidDirective) {
  EXPECT_FALSE(printf_result("%z", {"1"}));
}

}  // namespace
}  // namespace myzsh
