#ifndef ZTST_RUNNER_H
#define ZTST_RUNNER_H

#include <util/aliases.h>

namespace myzsh::ztst {

// One test case from a .ztst file. The format is a subset of zsh's own
// Test/*.ztst (see Test/README in the zsh sources):
//
//   %prep            code run before every test in the file
//   %test
//     code           indented by two spaces
//   <stdin line      input fed to the test
//   N:description    expected status (or '-' for any) and flags:
//                      d  don't compare stdout   D  don't compare stderr
//                      f  known failure (reported as skipped)
//   >stdout line
//   ?stderr line
//   F:note           printed when the test fails
//
// Cases are separated by blank lines; '#' at column 0 is a comment.
struct Case {
  str file;
  int index = 0;
  str description;
  str prep;
  str code;
  str input;
  opt<int> status;
  str out;
  str err;
  bool check_out = true;
  bool check_err = true;
  bool expected_failure = false;
  str note;
};

vec<Case> parse_file(const sf::path& path);
vec<Case> load_directory(const sf::path& directory);

struct Outcome {
  int status = 0;
  str out;
  str err;
  bool timed_out = false;
};

// Runs `shell -f -c PREP+CODE` in a fresh temporary directory with a
// minimal environment (PATH=/bin:/usr/bin, HOME=that directory, LC_ALL=C,
// TERM=xterm-256color). Error prefixes "<shell basename>:" in stderr are
// rewritten to "zsh:" so expectations can be shared between myzsh and zsh.
Outcome run_case(const Case& test_case, const sf::path& shell,
                 int timeout_seconds);

// A test name usable by gtest: file stem, index and description.
str case_name(const Case& test_case);

}  // namespace myzsh::ztst

#endif  // ZTST_RUNNER_H
