#include <gtest/gtest.h>

#include <history/history.h>
#include <support/test_support.h>

namespace myzsh {
namespace {

struct HistoryTest : ::testing::Test {
  History history;
  HistoryOptions options;

  void add_all(vec<strv> lines) {
    s::int64_t time = 1700000000;
    for (strv line : lines) {
      history.add(line, time++, options);
    }
  }

  str expand(strv line) {
    auto result = history.expand(line);
    EXPECT_TRUE(result) << line;
    return result ? result->line : str();
  }
};

TEST_F(HistoryTest, AddAndNumber) {
  add_all({"echo one", "echo two"});
  EXPECT_EQ(history.size(), 2u);
  EXPECT_EQ(history.first_number(), 1);
  EXPECT_EQ(history.next_number(), 3);
  EXPECT_EQ(history.get(2)->get().text, "echo two");
  EXPECT_FALSE(history.get(5));
}

TEST_F(HistoryTest, IgnoreDups) {
  options.ignore_dups = true;
  add_all({"a", "a", "b", "a"});
  EXPECT_EQ(history.size(), 3u);
}

TEST_F(HistoryTest, IgnoreAllDupsRemovesOlder) {
  options.ignore_all_dups = true;
  add_all({"a", "b", "a"});
  ASSERT_EQ(history.size(), 2u);
  EXPECT_EQ(history.entries()[0].text, "b");
  EXPECT_EQ(history.entries()[1].text, "a");
}

TEST_F(HistoryTest, IgnoreSpace) {
  options.ignore_space = true;
  add_all({" secret", "public"});
  EXPECT_EQ(history.size(), 1u);
}

TEST_F(HistoryTest, ReduceBlanks) {
  options.reduce_blanks = true;
  add_all({"echo   a    b  "});
  EXPECT_EQ(history.entries()[0].text, "echo a b");
}

TEST_F(HistoryTest, NoStore) {
  options.no_store = true;
  add_all({"history", "fc -l", "echo kept"});
  ASSERT_EQ(history.size(), 1u);
  EXPECT_EQ(history.entries()[0].text, "echo kept");
}

TEST_F(HistoryTest, LimitDropsOldest) {
  history.set_limit(2);
  add_all({"a", "b", "c"});
  ASSERT_EQ(history.size(), 2u);
  EXPECT_EQ(history.first_number(), 2);
  EXPECT_EQ(history.entries()[0].text, "b");
}

TEST_F(HistoryTest, Search) {
  add_all({"git status", "ls", "git commit", "make"});
  EXPECT_EQ(history.search_prefix("git", history.next_number()), 3);
  EXPECT_EQ(history.search_prefix("git", 3), 1);
  EXPECT_EQ(history.search_substring("stat", history.next_number()), 1);
  EXPECT_EQ(history.search_prefix("zzz", history.next_number()), s::nullopt);
}

TEST_F(HistoryTest, FileRoundTripExtended) {
  test::TempDir dir;
  options.extended = true;
  add_all({"echo one", "multi\nline"});
  sf::path file = dir / "hist";
  ASSERT_TRUE(history.write_file(file.string(), 100, false, options));
  EXPECT_EQ(test::read_file(file),
            ": 1700000000:0;echo one\n: 1700000001:0;multi\\\nline\n");
  History loaded;
  ASSERT_TRUE(loaded.read_file(file.string()));
  ASSERT_EQ(loaded.size(), 2u);
  EXPECT_EQ(loaded.entries()[1].text, "multi\nline");
  EXPECT_EQ(loaded.entries()[0].start_time, 1700000000);
}

TEST_F(HistoryTest, ReadsPlainFormat) {
  test::TempDir dir;
  sf::path file = dir.write("plain", "ls\ncd /tmp\n");
  ASSERT_TRUE(history.read_file(file.string()));
  EXPECT_EQ(history.size(), 2u);
}

TEST_F(HistoryTest, WriteRespectsLimitAndAppend) {
  test::TempDir dir;
  add_all({"a", "b", "c"});
  sf::path file = dir / "hist";
  history.write_file(file.string(), 2, false, options);
  EXPECT_EQ(test::read_file(file), "b\nc\n");
  history.write_file(file.string(), 1, true, options);
  EXPECT_EQ(test::read_file(file), "b\nc\nc\n");
}

TEST_F(HistoryTest, BangExpansionEvents) {
  add_all({"echo one two three", "ls -l /tmp", "git status"});
  EXPECT_EQ(expand("!!"), "git status");
  EXPECT_EQ(expand("!1"), "echo one two three");
  EXPECT_EQ(expand("!-2"), "ls -l /tmp");
  EXPECT_EQ(expand("!ls"), "ls -l /tmp");
  EXPECT_EQ(expand("!?stat?"), "git status");
  EXPECT_EQ(expand("sudo !!"), "sudo git status");
}

TEST_F(HistoryTest, BangExpansionWordDesignators) {
  add_all({"echo one two three"});
  EXPECT_EQ(expand("!!:0"), "echo");
  EXPECT_EQ(expand("!!:1"), "one");
  EXPECT_EQ(expand("!!:^"), "one");
  EXPECT_EQ(expand("!!:$"), "three");
  EXPECT_EQ(expand("!$"), "three");
  EXPECT_EQ(expand("!!:2-3"), "two three");
  EXPECT_EQ(expand("!!:*"), "one two three");
  EXPECT_EQ(expand("!!:2*"), "two three");
  EXPECT_EQ(expand("!!:-2"), "echo one two");
}

TEST_F(HistoryTest, BangExpansionModifiers) {
  add_all({"vi /usr/src/file.c"});
  EXPECT_EQ(expand("!!:$:h"), "/usr/src");
  EXPECT_EQ(expand("!!:$:t:r"), "file");
  EXPECT_EQ(expand("!!:s/vi/cat/"), "cat /usr/src/file.c");
  auto print_only = history.expand("!!:p");
  ASSERT_TRUE(print_only);
  EXPECT_TRUE(print_only->print_only);
}

TEST_F(HistoryTest, QuickSubstitution) {
  add_all({"echo hello world"});
  EXPECT_EQ(expand("^hello^bye"), "echo bye world");
  EXPECT_EQ(expand("^hello^bye^"), "echo bye world");
}

TEST_F(HistoryTest, LiteralBangs) {
  add_all({"x"});
  auto result = history.expand("echo ! '!!' \\!x != a");
  ASSERT_TRUE(result);
  EXPECT_FALSE(result->changed);
  EXPECT_EQ(result->line, "echo ! '!!' \\!x != a");
}

TEST_F(HistoryTest, CurrentLineReference) {
  EXPECT_EQ(expand("echo a !#"), "echo a echo a ");
}

TEST_F(HistoryTest, Errors) {
  add_all({"one"});
  auto missing = history.expand("!nomatch");
  ASSERT_FALSE(missing);
  EXPECT_EQ(missing.error(), "event not found: nomatch");
  auto bad_word = history.expand("!!:5");
  EXPECT_FALSE(bad_word);
}

TEST(SplitHistoryWords, QuotesAndOperators) {
  EXPECT_EQ(split_history_words("echo 'a b' c|wc -l;ls"),
            (vec<str>{"echo", "'a b'", "c", "|", "wc", "-l", ";", "ls"}));
}

}  // namespace
}  // namespace myzsh
