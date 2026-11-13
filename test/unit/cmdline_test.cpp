#include <gtest/gtest.h>

#include <shell/cmdline.h>

namespace myzsh {
namespace {

Invocation parse_ok(vec<str> argv) {
  auto result = parse_command_line(argv);
  EXPECT_TRUE(result) << (result ? "" : result.error());
  return result ? *result : Invocation{};
}

bool has_option(const Invocation& invocation, Option option, bool value) {
  for (const auto& [opt_name, opt_value] : invocation.options) {
    if (opt_name == option and opt_value == value) {
      return true;
    }
  }
  return false;
}

TEST(CommandLine, CommandMode) {
  Invocation inv = parse_ok({"zsh", "-c", "echo hi", "name", "a1"});
  EXPECT_EQ(inv.command, "echo hi");
  EXPECT_EQ(inv.argv0, "zsh");
  // After -c, the first argument becomes $0 and the rest positionals.
  EXPECT_EQ(inv.arguments, (vec<str>{"name", "a1"}));
}

TEST(CommandLine, ScriptMode) {
  Invocation inv = parse_ok({"zsh", "script.zsh", "x", "y"});
  EXPECT_EQ(inv.script, "script.zsh");
  EXPECT_EQ(inv.arguments, (vec<str>{"x", "y"}));
  EXPECT_FALSE(inv.read_stdin);
}

TEST(CommandLine, StdinMode) {
  Invocation none = parse_ok({"zsh"});
  EXPECT_TRUE(none.read_stdin);
  Invocation s_flag = parse_ok({"zsh", "-s", "a", "b"});
  EXPECT_TRUE(s_flag.read_stdin);
  EXPECT_EQ(s_flag.arguments, (vec<str>{"a", "b"}));
}

TEST(CommandLine, LetterOptions) {
  Invocation inv = parse_ok({"zsh", "-xe", "+f", "-c", "true"});
  EXPECT_TRUE(has_option(inv, Option::Xtrace, true));
  EXPECT_TRUE(has_option(inv, Option::Errexit, true));
  // -f is NO_RCS, so +f turns RCS back on.
  EXPECT_TRUE(has_option(inv, Option::Rcs, true));
}

TEST(CommandLine, LongOptions) {
  Invocation inv = parse_ok({"zsh", "-o", "extendedglob", "+o", "nomatch",
                             "--no-rcs", "--ksharrays", "-c", ":"});
  EXPECT_TRUE(has_option(inv, Option::Extendedglob, true));
  EXPECT_TRUE(has_option(inv, Option::Nomatch, false));
  EXPECT_TRUE(has_option(inv, Option::Rcs, false));
  EXPECT_TRUE(has_option(inv, Option::Ksharrays, true));
}

TEST(CommandLine, InteractiveAndLogin) {
  Invocation inv = parse_ok({"zsh", "-il"});
  EXPECT_TRUE(inv.login);
  EXPECT_TRUE(inv.interactive_requested());
  Invocation dash = parse_ok({"-zsh"});
  EXPECT_TRUE(dash.login);
}

TEST(CommandLine, EndOfOptions) {
  Invocation inv = parse_ok({"zsh", "--", "-script"});
  EXPECT_EQ(inv.script, "-script");
  Invocation dash = parse_ok({"zsh", "-", "-x"});
  EXPECT_EQ(dash.script, "-x");
}

TEST(CommandLine, EmulationFromName) {
  EXPECT_EQ(parse_ok({"sh", "-c", ":"}).emulation, Emulation::Sh);
  EXPECT_EQ(parse_ok({"/bin/ksh", "-c", ":"}).emulation, Emulation::Ksh);
  EXPECT_EQ(parse_ok({"zsh", "--emulate", "sh", "-c", ":"}).emulation,
            Emulation::Sh);
  EXPECT_EQ(parse_ok({"zsh", "-c", ":"}).emulation, Emulation::Zsh);
}

TEST(CommandLine, VersionAndHelp) {
  EXPECT_TRUE(parse_ok({"zsh", "--version"}).show_version);
  EXPECT_TRUE(parse_ok({"zsh", "--help"}).show_help);
}

TEST(CommandLine, Errors) {
  auto bad = parse_command_line({"zsh", "-y", "-j"});
  ASSERT_FALSE(bad);
  EXPECT_EQ(bad.error(), "bad option: -j");
  auto missing = parse_command_line({"zsh", "-o"});
  ASSERT_FALSE(missing);
  EXPECT_EQ(missing.error(), "string expected after -o");
  auto unknown = parse_command_line({"zsh", "-o", "nonsense"});
  ASSERT_FALSE(unknown);
  EXPECT_EQ(unknown.error(), "no such option: nonsense");
  auto no_command = parse_command_line({"zsh", "-c"});
  ASSERT_FALSE(no_command);
  EXPECT_EQ(no_command.error(), "string expected after -c");
}

}  // namespace
}  // namespace myzsh
