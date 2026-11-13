#ifndef COND_H
#define COND_H

#include <parse/ast.h>
#include <util/aliases.h>

namespace myzsh {

struct Shell;

// Unary file/string tests shared by [[ ]] and test/[ (Src/cond.c):
//   -a -e exists  -b block  -c char  -d dir  -f regular  -g setgid
//   -h -L symlink  -k sticky  -p fifo  -r -w -x access  -s non-empty
//   -u setuid  -S socket  -O owned by euid  -G group egid  -N modified since
//   read  -t fd is a tty  -n -z string  -o option set  -v param set
// Returns nullopt for an unknown operator.
opt<bool> unary_test(strv op, strv operand, Shell& shell);

// -nt -ot -ef and the integer comparisons -eq -ne -lt -le -gt -ge (whose
// operands are arithmetic expressions in [[ ]] but plain integers in test).
opt<bool> binary_file_test(strv op, strv left, strv right);

struct CondEvaluator {
  explicit CondEvaluator(Shell& shell) : mShell(shell) {}

  // [[ ... ]]: operands are expanded without splitting or globbing; the
  // right side of = == != is a pattern; =~ uses POSIX ERE (or PCRE with
  // RE_MATCH_PCRE) and sets $MATCH/$match (or BASH_REMATCH). Returns
  // 0 true, 1 false, 2 error.
  int evaluate(const ast::CondNode& node);

  // test / [ builtin: POSIX argument-count rules, then -a -o ! ( ).
  int evaluate_test(const vec<str>& args);

private:
  Shell& mShell;
};

}  // namespace myzsh

#endif  // COND_H
