#include <gtest/gtest.h>

#include <sys/stat.h>

#include <exec/command_hash.h>
#include <support/test_support.h>

namespace myzsh {
namespace {

struct CommandHashTest : ::testing::Test {
  test::TempDir dir;
  vec<str> path;

  void SetUp() override {
    for (strv name : {"bin1/tool", "bin2/tool", "bin2/other", "bin1/noexec"}) {
      sf::path file = dir.write(name, "#!/bin/sh\n");
      if (not str(name).ends_with("noexec")) {
        chmod(file.c_str(), 0755);
      }
    }
    dir.mkdir("bin1/subdir");
    path = {(dir / "bin1").string(), (dir / "bin2").string()};
  }
};

TEST_F(CommandHashTest, SearchPathFindsFirstExecutable) {
  EXPECT_EQ(search_path("tool", path), (dir / "bin1/tool").string());
  EXPECT_EQ(search_path("other", path), (dir / "bin2/other").string());
}

TEST_F(CommandHashTest, SearchPathSkipsNonExecutablesAndDirectories) {
  EXPECT_EQ(search_path("noexec", path), s::nullopt);
  EXPECT_EQ(search_path("subdir", path), s::nullopt);
  EXPECT_EQ(search_path("missing", path), s::nullopt);
}

TEST_F(CommandHashTest, NamesWithSlashAreNotSearched) {
  EXPECT_EQ(search_path("bin1/tool", path), s::nullopt);
}

TEST_F(CommandHashTest, FindCachesResult) {
  CommandHash hash;
  EXPECT_EQ(hash.find("tool", path), (dir / "bin1/tool").string());
  auto entries = hash.entries();
  ASSERT_EQ(entries.size(), 1u);
  EXPECT_EQ(entries[0].first, "tool");
}

TEST_F(CommandHashTest, ExplicitEntriesWinUntilRemoved) {
  CommandHash hash;
  hash.add("tool", (dir / "bin2/tool").string());
  EXPECT_EQ(hash.find("tool", path), (dir / "bin2/tool").string());
  EXPECT_TRUE(hash.remove("tool"));
  EXPECT_FALSE(hash.remove("tool"));
  EXPECT_EQ(hash.find("tool", path), (dir / "bin1/tool").string());
}

TEST_F(CommandHashTest, StaleEntryIsResearched) {
  CommandHash hash;
  hash.add("other", (dir / "gone/other").string());
  EXPECT_EQ(hash.find("other", path), (dir / "bin2/other").string());
}

TEST_F(CommandHashTest, FillAndClear) {
  CommandHash hash;
  hash.fill(path);
  auto entries = hash.entries();
  ASSERT_EQ(entries.size(), 2u);
  EXPECT_EQ(entries[0], (s::pair<str, str>{"other",
                                            (dir / "bin2/other").string()}));
  EXPECT_EQ(entries[1], (s::pair<str, str>{"tool",
                                            (dir / "bin1/tool").string()}));
  hash.clear();
  EXPECT_TRUE(hash.entries().empty());
}

}  // namespace
}  // namespace myzsh
