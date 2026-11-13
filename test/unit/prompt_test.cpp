#include <gtest/gtest.h>

#include <pwd.h>
#include <unistd.h>

#include <prompt/prompt.h>
#include <support/test_support.h>

namespace myzsh {
namespace {

struct PromptTest : ::testing::Test {
  uptr<Shell> shell = test::make_shell();

  void SetUp() override {
    shell->params.set_scalar("HOME", "/home/me");
    shell->params.set_scalar("PWD", "/home/me/gt/dir1/sub");
  }

  str prompt(strv text) { return expand_prompt(text, *shell).text; }
};

// Expected values from zsh 5.9 `print -P`.
TEST_F(PromptTest, Literals) {
  EXPECT_EQ(prompt("plain"), "plain");
  EXPECT_EQ(prompt("%%"), "%");
  EXPECT_EQ(prompt("%)"), ")");
}

TEST_F(PromptTest, Directories) {
  EXPECT_EQ(prompt("%~"), "~/gt/dir1/sub");
  EXPECT_EQ(prompt("%/"), "/home/me/gt/dir1/sub");
  EXPECT_EQ(prompt("%d"), "/home/me/gt/dir1/sub");
  EXPECT_EQ(prompt("%c"), "sub");
  EXPECT_EQ(prompt("%C"), "sub");
  EXPECT_EQ(prompt("%."), "sub");
  EXPECT_EQ(prompt("%1~"), "sub");
  EXPECT_EQ(prompt("%2/"), "dir1/sub");
  EXPECT_EQ(prompt("%2~"), "dir1/sub");
  EXPECT_EQ(prompt("%-1~"), "~");
}

TEST_F(PromptTest, NamedDirectoryAbbreviation) {
  shell->named_dirs.add("sp", "/home/me/gt");
  EXPECT_EQ(prompt("%~"), "~sp/dir1/sub");
}

TEST_F(PromptTest, HomeItself) {
  shell->params.set_scalar("PWD", "/home/me");
  EXPECT_EQ(prompt("%~"), "~");
  shell->params.set_scalar("PWD", "/");
  EXPECT_EQ(prompt("%~"), "/");
}

TEST_F(PromptTest, StatusAndConditionals) {
  shell->status = 0;
  EXPECT_EQ(prompt("%?|%(?.ok.bad)|%3(?.a.b)"), "0|ok|b");
  shell->status = 1;
  EXPECT_EQ(prompt("%?|%(?..fail)"), "1|fail");
  shell->status = 2;
  EXPECT_EQ(prompt("%2(?.a.b)"), "a");
  EXPECT_EQ(prompt("%(!.root.user)"), geteuid() == 0 ? "root" : "user");
}

TEST_F(PromptTest, JobsAndLevelConditionals) {
  shell->params.set_scalar("SHLVL", "3");
  EXPECT_EQ(prompt("%(1j.jobs.nojobs)|%(2L.deep.shallow)|%(5~.long.short)"),
            "nojobs|deep|short");
  EXPECT_EQ(prompt("%L"), "3");
  EXPECT_EQ(prompt("%j"), "0");
}

TEST_F(PromptTest, UserAndHost) {
  passwd* entry = getpwuid(geteuid());
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(prompt("%n"), entry->pw_name);
  char host[256] = {};
  gethostname(host, sizeof host);
  str full = host;
  EXPECT_EQ(prompt("%M"), full);
  EXPECT_EQ(prompt("%m"), full.substr(0, full.find('.')));
  EXPECT_EQ(prompt("%#"), geteuid() == 0 ? "#" : "%");
}

TEST_F(PromptTest, HistoryNumber) {
  EXPECT_EQ(prompt("%h"), prompt("%!"));
}

TEST_F(PromptTest, AttributesAndColours) {
  EXPECT_EQ(prompt("%B|%b|%U|%u|%S|%s"),
            "\x1b[1m|\x1b[0m|\x1b[4m|\x1b[24m|\x1b[7m|\x1b[27m");
  EXPECT_EQ(prompt("%F{red}r%f"), "\x1b[31mr\x1b[39m");
  EXPECT_EQ(prompt("%K{blue}%k"), "\x1b[44m\x1b[49m");
  EXPECT_EQ(prompt("%K{1}"), "\x1b[41m");
  EXPECT_EQ(prompt("%F{123}"), "\x1b[38;5;123m");
  EXPECT_EQ(prompt("%F{#ff0000}"), "\x1b[38;2;255;0;0m");
  EXPECT_EQ(prompt("%F{default}"), "\x1b[39m");
}

TEST_F(PromptTest, WidthIgnoresEscapes) {
  ExpandedPrompt result = expand_prompt("%F{red}ab%f%{xyz%}c", *shell);
  EXPECT_EQ(result.text, "\x1b[31mab\x1b[39mxyzc");
  EXPECT_EQ(result.width, 3u);
}

TEST_F(PromptTest, Truncation) {
  EXPECT_EQ(prompt("%10<..<abcdefghijklmnop%<<"), "..ijklmnop");
  EXPECT_EQ(prompt("%5>..>abcdefgh%>>|%3>>abcdef%>>|x"), "abc..|abc|x");
}

TEST_F(PromptTest, DateFormats) {
  str year = prompt("%D{%Y}");
  ASSERT_EQ(year.size(), 4u);
  EXPECT_EQ(year.substr(0, 2), "20");
  EXPECT_EQ(prompt("%D").size(), 8u);  // yy-mm-dd
  EXPECT_EQ(prompt("%*").size(), 8u);  // hh:mm:ss
}

TEST_F(PromptTest, PromptSubst) {
  shell->params.set_scalar("who", "x");
  EXPECT_EQ(prompt("$who"), "$who");
  shell->options.set(Option::Promptsubst, true);
  EXPECT_EQ(prompt("$who $(print y)"), "x y");
}

TEST_F(PromptTest, PromptPercentOff) {
  shell->options.set(Option::Promptpercent, false);
  EXPECT_EQ(prompt("%~"), "%~");
}

TEST_F(PromptTest, PromptBang) {
  shell->options.set(Option::Promptbang, true);
  EXPECT_EQ(prompt("!!"), "!");
  EXPECT_EQ(prompt("!"), prompt("%!"));
}

TEST(DisplayWidth, IgnoresCsiSequences) {
  EXPECT_EQ(display_width("abc"), 3u);
  EXPECT_EQ(display_width("\x1b[1mab\x1b[0m"), 2u);
  EXPECT_EQ(display_width("\xc3\xa9"), 1u);
}

}  // namespace
}  // namespace myzsh
