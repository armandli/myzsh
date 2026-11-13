#include <gtest/gtest.h>

#include <cstdlib>

#include <integration/ztst_runner.h>

namespace myzsh {
namespace {

// MYZSH_BIN overrides the shell under test; `make reference` points it at
// the real zsh to check that the expectations themselves are right.
sf::path shell_under_test() {
  const char* override_path = s::getenv("MYZSH_BIN");
  return override_path != nullptr and *override_path != '\0'
           ? sf::path(override_path)
           : sf::path(MYZSH_BINARY);
}

vec<ztst::Case> all_cases() {
  try {
    return ztst::load_directory(MYZSH_ZTST_DIR);
  } catch (const s::exception& error) {
    ADD_FAILURE() << error.what();
    return {};
  }
}

}  // namespace

namespace ztst {

void PrintTo(const Case& test_case, s::ostream* out) {
  *out << test_case.file << " #" << test_case.index << ": "
       << test_case.description;
}

}  // namespace ztst

namespace {

class Ztst : public ::testing::TestWithParam<ztst::Case> {};

TEST_P(Ztst, Run) {
  const ztst::Case& test_case = GetParam();
  if (test_case.expected_failure) {
    GTEST_SKIP() << "known failure: " << test_case.description;
  }
  ztst::Outcome outcome = ztst::run_case(test_case, shell_under_test(), 20);
  str context = "\n--- code:\n" + test_case.code + "--- stderr:\n" +
                outcome.err + test_case.note;
  ASSERT_FALSE(outcome.timed_out) << "timed out" << context;
  if (test_case.status) {
    EXPECT_EQ(outcome.status, *test_case.status) << context;
  }
  if (test_case.check_out) {
    EXPECT_EQ(outcome.out, test_case.out) << context;
  }
  if (test_case.check_err) {
    EXPECT_EQ(outcome.err, test_case.err) << context;
  }
}

INSTANTIATE_TEST_SUITE_P(Cases, Ztst, ::testing::ValuesIn(all_cases()),
                         [](const auto& info) {
                           return ztst::case_name(info.param);
                         });

}  // namespace
}  // namespace myzsh
