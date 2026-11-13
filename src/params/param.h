#ifndef PARAM_H
#define PARAM_H

#include <cstdint>
#include <functional>
#include <map>
#include <variant>

#include <util/aliases.h>

namespace myzsh {

enum class ParamType : int {
  Scalar,
  Integer,
  Float,
  Array,
  Assoc,
  Nameref,
};

// typeset attribute flags (Src/zsh.h PM_*).
enum ParamFlag : s::uint32_t {
  PF_NONE = 0,
  PF_EXPORT = 1u << 0,      // -x
  PF_READONLY = 1u << 1,    // -r
  PF_LOWER = 1u << 2,       // -l
  PF_UPPER = 1u << 3,       // -u (scalars)
  PF_LEFT = 1u << 4,        // -L n
  PF_RIGHT_BLANK = 1u << 5, // -R n
  PF_RIGHT_ZERO = 1u << 6,  // -Z n
  PF_UNIQUE = 1u << 7,      // -U (arrays)
  PF_TAGGED = 1u << 8,      // -t
  PF_HIDE = 1u << 9,        // -h  special-ness hidden in local scope
  PF_HIDE_VALUE = 1u << 10, // -H  value not shown by typeset
  PF_EXPONENT = 1u << 11,   // -E  float printed in exponent form
  PF_FIXED = 1u << 12,      // -F  float printed fixed-point
  PF_TIED = 1u << 13,       // -T  scalar/array pair like PATH/path
  PF_SPECIAL = 1u << 14,    // backed by SpecialParam hooks
  PF_UNSET = 1u << 15,      // declared (local) but unset
  PF_GLOBAL = 1u << 16,     // -g
};

using Assoc = s::map<str, str>;
using ParamValue = s::variant<str, s::int64_t, double, vec<str>, Assoc>;

struct Param {
  str name;
  ParamType type = ParamType::Scalar;
  s::uint32_t flags = PF_NONE;
  // Integer: output base (typeset -i 16). Float: precision. Justify: width.
  int base = 10;
  int width = 0;
  // Function nesting depth at which it was declared (0 = global).
  int level = 0;
  ParamValue value = str();
  // For PF_TIED: name of the partner parameter, and the join character.
  str tied_with;
  char tie_separator = ':';

  bool has(ParamFlag flag) const { return (flags & flag) != 0; }
};

// Hooks for parameters computed on access ($RANDOM, $SECONDS, $PWD, $?...).
struct SpecialParam {
  ParamType type = ParamType::Scalar;
  s::function<ParamValue()> get;
  // Absent for read-only specials.
  s::function<void(const ParamValue&)> set;
  s::function<void()> unset;
};

// Parameter storage with dynamic scoping (Src/params.c). `local`/typeset in a
// function pushes a new definition that is popped when the scope ends, the
// shadowed outer definition becoming visible again.
struct ParamTable {
  opt<s::reference_wrapper<const Param>> find(strv name) const;
  bool is_set(strv name) const;

  // Scalar value of any parameter: arrays joined with a space, integers and
  // floats formatted per their base/precision, specials via get().
  opt<str> get_scalar(strv name) const;
  // Array value; a scalar becomes a one-element array.
  opt<vec<str>> get_array(strv name) const;
  opt<Assoc> get_assoc(strv name) const;
  opt<s::int64_t> get_integer(strv name) const;

  // The setters honour the parameter's type and attributes: assigning to an
  // integer evaluates arithmetic (through mArithmetic), -l/-u case-folds,
  // -L/-R/-Z justify, -U dedups, tied pairs stay in sync, and an exported
  // parameter is updated in the environment list. Assigning to a readonly
  // parameter throws ShellError("read-only variable: NAME").
  void set_scalar(strv name, strv value);
  void set_integer(strv name, s::int64_t value);
  void set_float(strv name, double value);
  void set_array(strv name, vec<str> value);
  void set_assoc(strv name, Assoc value);
  // name+=value: string concatenation for scalars, element append for
  // arrays, arithmetic addition for integers.
  void append_scalar(strv name, strv value);
  void append_array(strv name, const vec<str>& value);

  // Returns false if the name was not set; throws for readonly.
  bool unset(strv name);

  // typeset: create or convert a parameter in the current scope.
  Param& declare(strv name, ParamType type, s::uint32_t flags,
                 bool local_scope);
  void set_flags(strv name, s::uint32_t add, s::uint32_t remove);

  void push_scope();
  void pop_scope();
  int scope_level() const { return mLevel; }

  void add_special(strv name, SpecialParam special, s::uint32_t flags);

  // Sorted parameter names (for `typeset`/`set` listings and completion).
  vec<str> names() const;
  // "NAME=value" for every exported parameter, for execve().
  vec<str> environment() const;
  void import_environment(char** envp);

  // Integer parameters evaluate their right-hand side; the shell installs
  // the arithmetic evaluator here to avoid a dependency cycle.
  void set_arithmetic(s::function<s::int64_t(strv)> evaluate) {
    mArithmetic = s::move(evaluate);
  }

protected:
  Param& lookup_or_create(strv name);

private:
  // Newest definition last; earlier entries are shadowed by `local`.
  umap<str, vec<Param>> mParams;
  umap<str, SpecialParam> mSpecials;
  int mLevel = 0;
  s::function<s::int64_t(strv)> mArithmetic;
};

// Formats a float the way zsh prints $((x)): "3." for 3.0, 17 significant
// digits otherwise (1/3.0 -> 0.33333333333333331).
str format_float(double value);
// typeset -E / -F formatting with the given precision.
str format_float(double value, bool exponent, int precision);
// Integer in a given base, zsh syntax ("16#FF"); base 10 prints plainly.
str format_integer(s::int64_t value, int base, bool with_prefix = true);

}  // namespace myzsh

#endif  // PARAM_H
