#ifndef SUBSCRIPT_H
#define SUBSCRIPT_H

#include <cstdint>

#include <params/param.h>
#include <util/aliases.h>

namespace myzsh {

struct Shell;

// A parsed [...] subscript (Src/params.c getindex/getarg).
struct Subscript {
  enum class Kind : int {
    Index,  // [n]
    Range,  // [n,m]
    All,    // [@] or [*]
    Search, // [(r)pat] [(R)pat] [(i)pat] [(I)pat] and the assoc flags
  };

  Kind kind = Kind::Index;
  str first;  // raw arithmetic (or pattern / key for Search & assoc)
  str second; // Range end
  bool star = false; // [*] rather than [@]

  // Subscript flags in (...) before the expression.
  bool reverse = false;        // r: first match, value
  bool reverse_last = false;   // R: last match, value
  bool index_first = false;    // i: first match, index
  bool index_last = false;     // I: last match, index
  bool words = false;          // w: scalar indexed by words
  str word_separator;          // s:sep: used with w
  bool keys = false;           // k: assoc keys are patterns, first value
  bool all_keys = false;       // K: every value whose key-pattern matches
  bool exact = false;          // e: no pattern characters
  int nth = 1;                 // n:N:
  int begin = 0;               // b:N:
};

Subscript parse_subscript(strv text);

// Evaluation needs the shell for arithmetic and pattern matching.
struct SubscriptEvaluator {
  explicit SubscriptEvaluator(Shell& shell) : mShell(shell) {}

  // Indices are 1-based, negative counts from the end, 0 is invalid (empty)
  // unless KSH_ZERO_SUBSCRIPT; out-of-range reads give empty.
  vec<str> read_array(const vec<str>& array, const Subscript& subscript);
  str read_scalar(strv value, const Subscript& subscript);
  vec<str> read_assoc(const Assoc& assoc, const Subscript& subscript);

  // Assignment to an element or slice: a[2]=x, a[2,3]=(x y z), a[-1]=(),
  // assigning past the end pads with empty elements.
  void assign_array(vec<str>& array, const Subscript& subscript,
                    const vec<str>& value);
  void assign_scalar(str& value, const Subscript& subscript, strv text);

protected:
  s::int64_t eval_index(strv expression);

private:
  Shell& mShell;
};

}  // namespace myzsh

#endif  // SUBSCRIPT_H
