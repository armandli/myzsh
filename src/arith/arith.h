#ifndef ARITH_H
#define ARITH_H

#include <cstdint>
#include <variant>

#include <util/aliases.h>

namespace myzsh {

struct Shell;

struct MathNumber {
  s::variant<s::int64_t, double> value = s::int64_t{0};

  MathNumber() = default;
  MathNumber(s::int64_t integer) : value(integer) {}
  MathNumber(double real) : value(real) {}

  bool is_float() const { return s::holds_alternative<double>(value); }
  s::int64_t as_integer() const;
  double as_float() const;

  bool operator==(const MathNumber&) const = default;
};

// Result of evaluation plus any [#base] output-format request.
struct MathResult {
  MathNumber number;
  // 0 when no [#N] was given.
  int output_base = 0;
  // [##N]: print without the "N#" prefix.
  bool suppress_base_prefix = false;
};

// Arithmetic evaluation (Src/math.c): C operator precedence (or
// C_PRECEDENCES off: zsh's own table) plus ** and the logical ^^, ternary,
// assignment ops, comma, ++/--, integer and float arithmetic, bases
// (16#ff, 0x1f, 0b101; leading-zero octal only with OCTAL_ZEROES), #'c'
// and ##c character codes, parameter references with or without $, and
// math functions registered by modules (zsh/mathfunc).
// Errors throw ShellError: "division by zero", "bad math expression: ...".
struct ArithEvaluator {
  explicit ArithEvaluator(Shell& shell) : mShell(shell) {}

  MathResult evaluate(strv expression);
  // $(( )) output text, honouring [#N] and float formatting.
  str evaluate_to_string(strv expression);

private:
  Shell& mShell;
};

}  // namespace myzsh

#endif  // ARITH_H
