#include <cond/cond.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

opt<bool> unary_test(strv op, strv operand, Shell& shell) {
  // TODO(impl): stat()/lstat()/access(); -t takes an fd number.
  todo("unary_test");
}

opt<bool> binary_file_test(strv op, strv left, strv right) {
  // TODO(impl)
  todo("binary_file_test");
}

int CondEvaluator::evaluate(const ast::CondNode& node) {
  // TODO(impl): string < > compare in the C locale.
  todo("CondEvaluator::evaluate");
}

int CondEvaluator::evaluate_test(const vec<str>& args) {
  // TODO(impl): 0 args false; 1 arg non-empty; 2 args "! x" or unary;
  // 3 args binary, "! a b", "( x )"; more: full precedence parse.
  // Errors print "test: ..." and return 2.
  todo("CondEvaluator::evaluate_test");
}

}  // namespace myzsh
