#include <gtest/gtest.h>

#include <sys/stat.h>

#include <glob/glob.h>
#include <support/test_support.h>

namespace myzsh {
namespace {

using Words = vec<str>;

// Mirrors the tree used to capture expected results from zsh 5.9:
//   a.c b.c(+x) c.h .hidden file1 file2 file10 link.c -> a.c
//   dir1/x.c dir1/sub/y.c dir2/
struct GlobTest : ::testing::Test {
  test::TempDir dir;

  void SetUp() override {
    for (strv name : {"a.c", "b.c", "c.h", ".hidden", "file1", "file2",
                      "file10", "dir1/x.c", "dir1/sub/y.c"}) {
      dir.write(name, "data");
    }
    dir.mkdir("dir2");
    sf::create_symlink("a.c", dir / "link.c");
    chmod((dir / "b.c").c_str(), 0755);
  }

  GlobOptions options() const {
    GlobOptions result;
    result.cwd = dir.path().string();
    return result;
  }

  Words glob(strv pattern, GlobOptions opts) {
    return Globber(s::move(opts)).expand(QString(pattern));
  }
  Words glob(strv pattern) { return glob(pattern, options()); }
};

TEST_F(GlobTest, Star) {
  EXPECT_EQ(glob("*.c"), (Words{"a.c", "b.c", "link.c"}));
}

TEST_F(GlobTest, HiddenFilesNeedDotOrGlobDots) {
  EXPECT_EQ(glob(".h*"), Words{".hidden"});
  GlobOptions opts = options();
  opts.glob_dots = true;
  Words all = glob("*", opts);
  EXPECT_EQ(all.front(), ".hidden");
}

TEST_F(GlobTest, NoMatchGivesEmpty) {
  EXPECT_EQ(glob("*.zz"), Words{});
}

TEST_F(GlobTest, CharacterClassAndQuestion) {
  EXPECT_EQ(glob("[a-b].c"), (Words{"a.c", "b.c"}));
  EXPECT_EQ(glob("[!a].c"), Words{"b.c"});
  EXPECT_EQ(glob("file?"), (Words{"file1", "file2"}));
}

TEST_F(GlobTest, DirectoryComponents) {
  EXPECT_EQ(glob("dir*/*.c"), Words{"dir1/x.c"});
  EXPECT_EQ(glob("*/sub"), Words{"dir1/sub"});
}

TEST_F(GlobTest, RecursiveGlob) {
  EXPECT_EQ(glob("**/*.c"),
            (Words{"a.c", "b.c", "dir1/sub/y.c", "dir1/x.c", "link.c"}));
}

TEST_F(GlobTest, AbsolutePattern) {
  str pattern = dir.path().string() + "/c.*";
  EXPECT_EQ(glob(pattern), Words{dir.path().string() + "/c.h"});
}

TEST_F(GlobTest, NumericSort) {
  EXPECT_EQ(glob("file*"), (Words{"file1", "file10", "file2"}));
  GlobOptions opts = options();
  opts.numeric_sort = true;
  EXPECT_EQ(glob("file*", opts), (Words{"file1", "file2", "file10"}));
}

TEST_F(GlobTest, MarkDirs) {
  GlobOptions opts = options();
  opts.mark_dirs = true;
  EXPECT_EQ(glob("dir*", opts), (Words{"dir1/", "dir2/"}));
}

TEST_F(GlobTest, ExtendedGlob) {
  GlobOptions opts = options();
  opts.extended_glob = true;
  EXPECT_EQ(glob("^*.c", opts),
            (Words{"c.h", "dir1", "dir2", "file1", "file10", "file2"}));
  EXPECT_EQ(glob("*~*.h", opts),
            (Words{"a.c", "b.c", "dir1", "dir2", "file1", "file10", "file2",
                   "link.c"}));
  EXPECT_EQ(glob("file<1-5>", opts), (Words{"file1", "file2"}));
  EXPECT_EQ(glob("(#i)A.C", opts), Words{"a.c"});
}

TEST_F(GlobTest, Alternation) {
  EXPECT_EQ(glob("(a|b).c"), (Words{"a.c", "b.c"}));
}

TEST_F(GlobTest, KshGlob) {
  GlobOptions opts = options();
  opts.ksh_glob = true;
  EXPECT_EQ(glob("@(a|c).*", opts), (Words{"a.c", "c.h"}));
}

TEST_F(GlobTest, QualifierFileTypes) {
  EXPECT_EQ(glob("*(.)"),
            (Words{"a.c", "b.c", "c.h", "file1", "file10", "file2"}));
  EXPECT_EQ(glob("*(/)"), (Words{"dir1", "dir2"}));
  EXPECT_EQ(glob("*(@)"), Words{"link.c"});
  EXPECT_EQ(glob("*(*)"), Words{"b.c"});
  EXPECT_EQ(glob("*(^/)").size(), 7u);
}

TEST_F(GlobTest, QualifierFlags) {
  EXPECT_EQ(glob("*(D.)"),
            (Words{".hidden", "a.c", "b.c", "c.h", "file1", "file10",
                   "file2"}));
  EXPECT_EQ(glob("file*(n)"), (Words{"file1", "file2", "file10"}));
  EXPECT_EQ(glob("*(/M)"), (Words{"dir1/", "dir2/"}));
  EXPECT_EQ(glob("*.c(:r)"), (Words{"a", "b", "link"}));
  EXPECT_EQ(glob("*.zz(N)"), Words{});
}

TEST_F(GlobTest, QualifierSortAndSlice) {
  EXPECT_EQ(glob("file*(On)"), (Words{"file2", "file10", "file1"}));
  EXPECT_EQ(glob("file*(on[1,2])"), (Words{"file1", "file10"}));
  EXPECT_EQ(glob("file*(n[-1])"), Words{"file10"});
}

TEST_F(GlobTest, QualifierSize) {
  dir.write("big", str(2048, 'x'));
  EXPECT_EQ(glob("big(Lk+1)"), Words{"big"});
  EXPECT_EQ(glob("a.c(L+100)"), Words{});
}

TEST_F(GlobTest, QualifierEvalCode) {
  Globber globber(options());
  globber.set_qualifier_evaluator([](strv path, strv code) {
    return code == "keep" and path.ends_with(".h");
  });
  EXPECT_EQ(globber.expand(QString("*(e:keep:)")), Words{"c.h"});
}

TEST_F(GlobTest, BareGlobQualDisabled) {
  GlobOptions opts = options();
  opts.bare_glob_qual = false;
  // (.) is then a pattern group matching a literal '.' suffix.
  EXPECT_EQ(glob("*(.)", opts), Words{});
}

TEST(ParseGlobQualifiers, Parses) {
  auto quals = parse_glob_qualifiers(".om[1,3]");
  ASSERT_TRUE(quals);
  EXPECT_EQ(quals->tests, vec<str>{"."});
  EXPECT_EQ(quals->sort_keys, "m");
  EXPECT_FALSE(quals->descending);
  ASSERT_TRUE(quals->range);
  EXPECT_EQ(quals->range->first, 1);
  EXPECT_EQ(quals->range->second, 3);
  auto flags = parse_glob_qualifiers("NDM");
  ASSERT_TRUE(flags);
  EXPECT_TRUE(flags->null_glob);
  EXPECT_TRUE(flags->glob_dots);
  EXPECT_TRUE(flags->mark_dirs);
  auto mods = parse_glob_qualifiers(":t:r");
  ASSERT_TRUE(mods);
  EXPECT_EQ(mods->modifiers, ":t:r");
  auto code = parse_glob_qualifiers("e:'[[ -s $REPLY ]]':");
  ASSERT_TRUE(code);
  EXPECT_EQ(code->eval_code, vec<str>{"'[[ -s $REPLY ]]'"});
}

TEST(ParseGlobQualifiers, RejectsNonQualifiers) {
  EXPECT_FALSE(parse_glob_qualifiers("a|b"));
}

}  // namespace
}  // namespace myzsh
