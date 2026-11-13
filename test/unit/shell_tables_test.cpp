#include <gtest/gtest.h>

#include <pwd.h>
#include <unistd.h>

#include <shell/alias_table.h>
#include <shell/named_dirs.h>

namespace myzsh {
namespace {

TEST(AliasTable, DefineFindRemove) {
  AliasTable aliases;
  aliases.define("ll", "ls -l", AliasKind::Regular);
  auto found = aliases.find("ll", AliasKind::Regular);
  ASSERT_TRUE(found);
  EXPECT_EQ(found->get().text, "ls -l");
  EXPECT_TRUE(aliases.remove("ll", AliasKind::Regular));
  EXPECT_FALSE(aliases.find("ll", AliasKind::Regular));
  EXPECT_FALSE(aliases.remove("ll", AliasKind::Regular));
}

TEST(AliasTable, ExpansionDependsOnPosition) {
  AliasTable aliases;
  aliases.define("ll", "ls -l", AliasKind::Regular);
  aliases.define("G", "| grep", AliasKind::Global);
  EXPECT_EQ(aliases.expansion("ll", true), "ls -l");
  EXPECT_EQ(aliases.expansion("ll", false), s::nullopt);
  EXPECT_EQ(aliases.expansion("G", false), "| grep");
  EXPECT_EQ(aliases.expansion("G", true), "| grep");
}

TEST(AliasTable, DisabledAliasesDoNotExpand) {
  AliasTable aliases;
  aliases.define("ll", "ls -l", AliasKind::Regular);
  aliases.set_disabled("ll", AliasKind::Regular, true);
  EXPECT_EQ(aliases.expansion("ll", true), s::nullopt);
  aliases.set_disabled("ll", AliasKind::Regular, false);
  EXPECT_EQ(aliases.expansion("ll", true), "ls -l");
}

TEST(AliasTable, SuffixAliasesAreSeparate) {
  AliasTable aliases;
  aliases.define("txt", "less", AliasKind::Suffix);
  EXPECT_TRUE(aliases.find("txt", AliasKind::Suffix));
  EXPECT_FALSE(aliases.find("txt", AliasKind::Regular));
  EXPECT_EQ(aliases.expansion("txt", true), s::nullopt);
}

TEST(AliasTable, Listing) {
  AliasTable aliases;
  aliases.define("ll", "ls -l", AliasKind::Regular);
  aliases.define("a", "b", AliasKind::Regular);
  EXPECT_EQ(aliases.listing(AliasKind::Regular, false),
            (vec<str>{"a=b", "ll='ls -l'"}));
  EXPECT_EQ(aliases.listing(AliasKind::Regular, true),
            (vec<str>{"alias a=b", "alias ll='ls -l'"}));
  aliases.define("G", "| grep", AliasKind::Global);
  vec<str> as_commands = aliases.listing(AliasKind::Global, true);
  EXPECT_EQ(as_commands, vec<str>{"alias -g G='| grep'"});
}

TEST(AliasTable, EndsWithBlank) {
  EXPECT_TRUE(AliasTable::ends_with_blank("sudo "));
  EXPECT_FALSE(AliasTable::ends_with_blank("sudo"));
}

TEST(NamedDirTable, AddFindRemove) {
  NamedDirTable dirs;
  dirs.add("proj", "/src/project");
  EXPECT_EQ(dirs.find("proj"), "/src/project");
  EXPECT_TRUE(dirs.remove("proj"));
  EXPECT_EQ(dirs.find("proj"), s::nullopt);
}

TEST(NamedDirTable, AbbreviatePrefersLongest) {
  NamedDirTable dirs;
  dirs.add("src", "/home/me/src");
  dirs.add("proj", "/home/me/src/project");
  auto abbrev = dirs.abbreviate("/home/me/src/project/lib", "/home/me");
  ASSERT_TRUE(abbrev);
  EXPECT_EQ(abbrev->name, "proj");
  EXPECT_EQ(abbrev->length, str("/home/me/src/project").size());
  auto home = dirs.abbreviate("/home/me/docs", "/home/me");
  ASSERT_TRUE(home);
  EXPECT_EQ(home->name, "");
  EXPECT_FALSE(dirs.abbreviate("/etc", "/home/me"));
  // Only on a component boundary.
  EXPECT_FALSE(dirs.abbreviate("/home/meadow", "/home/me"));
}

TEST(NamedDirTable, UserHome) {
  passwd* entry = getpwuid(getuid());
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(NamedDirTable::user_home(entry->pw_name), str(entry->pw_dir));
  EXPECT_EQ(NamedDirTable::user_home("no_such_user_xyz"), s::nullopt);
}

TEST(NamedDirTable, EntriesSorted) {
  NamedDirTable dirs;
  dirs.add("b", "/b");
  dirs.add("a", "/a");
  auto entries = dirs.entries();
  ASSERT_EQ(entries.size(), 2u);
  EXPECT_EQ(entries[0].first, "a");
}

}  // namespace
}  // namespace myzsh
