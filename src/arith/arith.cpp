#include <arith/arith.h>

#include <shell/shell.h>
#include <util/error.h>
#include <util/todo.h>

namespace myzsh {

s::int64_t MathNumber::as_integer() const {
  // TODO(impl): truncate floats toward zero.
  todo("MathNumber::as_integer");
}

double MathNumber::as_float() const {
  // TODO(impl)
  todo("MathNumber::as_float");
}

MathResult ArithEvaluator::evaluate(strv expression) {
  // TODO(impl): operator-precedence (Pratt) parser that evaluates as it
  // parses, with a "no-eval" flag for the untaken side of && || ?: (zsh's
  // noeval). Variables hold either numbers or expression text which is
  // evaluated recursively; unset variables are 0. Integer ops wrap like
  // int64; mixing with a float promotes to float. FORCE_FLOAT promotes
  // constants.
  todo("ArithEvaluator::evaluate");
}

str ArithEvaluator::evaluate_to_string(strv expression) {
  // TODO(impl): format_integer / format_float from params/param.h.
  todo("ArithEvaluator::evaluate_to_string");
}

}  // namespace myzsh
