#include <gtest/gtest.h>

#include <sys/stat.h>

#include <cond/cond.h>
#include <parse/parser.h>
#include <support/test_support.h>

namespace myzsh {
namespace {

struct CondTest : ::testing::Test {
  uptr<Shell> shell = test::make_shell();
  CondEvaluator cond{*shell};
  test::TempDir dir;

  void SetUp() override {
    dir.write("file", "content");
    dir.write("empty");
    dir.mkdir("subdir");
    sf::create_symlink("file", dir / "link");
    chmod((dir / "file").c_str(), 0755);
  }

  sf::path path(strv name) const { return dir / name; }

  // Evaluates the [[ ... ]] in source.
  int eval(strv source) {
    auto list = parse(source);
    EXPECT_TRUE(list) << source;
    if (not list) {
      return 2;
    }
    const auto& command =
      list->items.at(0).sublist.items.at(0).pipeline.commands.at(0);
    return cond.evaluate(*s::get<ast::Cond>(command.node).expression);
  }
};

TEST_F(CondTest, UnaryFileTests) {
  EXPECT_EQ(unary_test("-e", path("file").string(), *shell), true);
  EXPECT_EQ(unary_test("-a", path("file").string(), *shell), true);
  EXPECT_EQ(unary_test("-f", path("file").string(), *shell), true);
  EXPECT_EQ(unary_test("-f", path("subdir").string(), *shell), false);
  EXPECT_EQ(unary_test("-d", path("subdir").string(), *shell), true);
  EXPECT_EQ(unary_test("-L", path("link").string(), *shell), true);
  EXPECT_EQ(unary_test("-h", path("file").string(), *shell), false);
  EXPECT_EQ(unary_test("-s", path("file").string(), *shell), true);
  EXPECT_EQ(unary_test("-s", path("empty").string(), *shell), false);
  EXPECT_EQ(unary_test("-x", path("file").string(), *shell), true);
  EXPECT_EQ(unary_test("-r", path("file").string(), *shell), true);
  EXPECT_EQ(unary_test("-w", path("file").string(), *shell), true);
  EXPECT_EQ(unary_test("-O", path("file").string(), *shell), true);
  EXPECT_EQ(unary_test("-e", path("missing").string(), *shell), false);
  EXPECT_EQ(unary_test("-p", path("file").string(), *shell), false);
  EXPECT_EQ(unary_test("-S", path("file").string(), *shell), false);
  EXPECT_EQ(unary_test("-c", "/dev/null", *shell), true);
}

TEST_F(CondTest, UnaryStringAndShellTests) {
  EXPECT_EQ(unary_test("-n", "x", *shell), true);
  EXPECT_EQ(unary_test("-n", "", *shell), false);
  EXPECT_EQ(unary_test("-z", "", *shell), true);
  shell->params.set_scalar("defined", "1");
  EXPECT_EQ(unary_test("-v", "defined", *shell), true);
  EXPECT_EQ(unary_test("-v", "undefined_xyz", *shell), false);
  EXPECT_EQ(unary_test("-o", "glob", *shell), true);
  EXPECT_EQ(unary_test("-o", "extendedglob", *shell), false);
  EXPECT_EQ(unary_test("-Q", "x", *shell), s::nullopt);
}

TEST_F(CondTest, BinaryFileTests) {
  EXPECT_EQ(binary_file_test("-ef", path("file").string(),
                             path("link").string()), true);
  EXPECT_EQ(binary_file_test("-ef", path("file").string(),
                             path("empty").string()), false);
  sf::last_write_time(path("empty"),
                      sf::last_write_time(path("file")) + sc::hours(1));
  EXPECT_EQ(binary_file_test("-nt", path("empty").string(),
                             path("file").string()), true);
  EXPECT_EQ(binary_file_test("-ot", path("empty").string(),
                             path("file").string()), false);
  EXPECT_EQ(binary_file_test("-eq", "3", "3"), true);
  EXPECT_EQ(binary_file_test("-lt", "2", "10"), true);
  EXPECT_EQ(binary_file_test("-ge", "2", "10"), false);
}

TEST_F(CondTest, StringAndPatternComparisons) {
  EXPECT_EQ(eval("[[ abc = a* ]]"), 0);
  EXPECT_EQ(eval("[[ abc == a?c ]]"), 0);
  EXPECT_EQ(eval("[[ abc = \"a*\" ]]"), 1);
  EXPECT_EQ(eval("[[ abc != b* ]]"), 0);
  EXPECT_EQ(eval("[[ b > a ]]"), 0);
  EXPECT_EQ(eval("[[ a < b ]]"), 0);
  EXPECT_EQ(eval("[[ -n x && -z '' ]]"), 0);
  EXPECT_EQ(eval("[[ x ]]"), 0);
  EXPECT_EQ(eval("[[ '' ]]"), 1);
}

TEST_F(CondTest, ArithmeticComparisonsEvaluateExpressions) {
  EXPECT_EQ(eval("[[ 2 -lt 10 ]]"), 0);
  EXPECT_EQ(eval("[[ 10 -eq 1+9 ]]"), 0);
  EXPECT_EQ(eval("[[ 3 -gt 4 ]]"), 1);
}

TEST_F(CondTest, LogicalOperatorsAndGrouping) {
  EXPECT_EQ(eval("[[ ! -z x ]]"), 0);
  EXPECT_EQ(eval("[[ -z x || -n x ]]"), 0);
  EXPECT_EQ(eval("[[ ( -z x || -n x ) && a = b ]]"), 1);
}

TEST_F(CondTest, RegexMatchSetsMatchArrays) {
  EXPECT_EQ(eval("[[ abc =~ ^a(b)c$ ]]"), 0);
  EXPECT_EQ(shell->params.get_scalar("MATCH"), "abc");
  EXPECT_EQ(shell->params.get_array("match"), vec<str>{"b"});
  EXPECT_EQ(eval("[[ abc =~ ^x ]]"), 1);
}

TEST_F(CondTest, BashRematch) {
  shell->options.set(Option::Bashrematch, true);
  EXPECT_EQ(eval("[[ abc =~ a(b)c ]]"), 0);
  EXPECT_EQ(shell->params.get_array("BASH_REMATCH"),
            (vec<str>{"abc", "b"}));
}

TEST_F(CondTest, InvalidRegexIsError) {
  EXPECT_EQ(eval("[[ a =~ '(' ]]"), 2);
}

TEST_F(CondTest, TestBuiltinArgumentCounts) {
  EXPECT_EQ(cond.evaluate_test({}), 1);
  EXPECT_EQ(cond.evaluate_test({"x"}), 0);
  EXPECT_EQ(cond.evaluate_test({""}), 1);
  EXPECT_EQ(cond.evaluate_test({"!", "x"}), 1);
  EXPECT_EQ(cond.evaluate_test({"-z", ""}), 0);
  EXPECT_EQ(cond.evaluate_test({"a", "=", "a"}), 0);
  EXPECT_EQ(cond.evaluate_test({"a", "!=", "a"}), 1);
  EXPECT_EQ(cond.evaluate_test({"(", "x", ")"}), 0);
  EXPECT_EQ(cond.evaluate_test({"!", "a", "=", "b"}), 0);
}

TEST_F(CondTest, TestBuiltinAndOr) {
  EXPECT_EQ(cond.evaluate_test({"1", "-eq", "1", "-a", "2", "-gt", "1"}), 0);
  EXPECT_EQ(cond.evaluate_test({"1", "-eq", "2", "-o", "x", "=", "x"}), 0);
  EXPECT_EQ(cond.evaluate_test({"(", "x", "=", "x", ")"}), 0);
}

TEST_F(CondTest, TestBuiltinPatternsAreLiteral) {
  EXPECT_EQ(cond.evaluate_test({"abc", "=", "a*"}), 1);
}

TEST_F(CondTest, TestBuiltinErrors) {
  test::Captured result = test::capture([&] {
    return cond.evaluate_test({"1", "-eq", "x"});
  });
  EXPECT_EQ(result.status, 2);
  EXPECT_FALSE(result.err.empty());
}

}  // namespace
}  // namespace myzsh
