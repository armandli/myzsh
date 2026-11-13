#ifndef PARAM_EXPAND_H
#define PARAM_EXPAND_H

#include <expected>

#include <expand/modifiers.h>
#include <util/aliases.h>
#include <util/error.h>

namespace myzsh {

struct Shell;

// One (X) flag of ${(X)name}; flags with arguments use any delimiter pair:
// (s:,:) (j.-.) (l:5::0:) (r:5::.:).
struct ParamFlagSpec {
  char flag = 0;
  vec<str> arguments;
  // Repetition count for flags that stack: (qq) (qqq) (ff)...
  int repeat = 1;

  bool operator==(const ParamFlagSpec&) const = default;
};

enum class ParamOp : int {
  None,
  Default,         // ${x-w}  ${x:-w}
  Assign,          // ${x=w}  ${x:=w}  ${x::=w}
  Error,           // ${x?w}  ${x:?w}
  Alternate,       // ${x+w}  ${x:+w}
  RemovePrefix,    // ${x#p}  ${x##p}
  RemoveSuffix,    // ${x%p}  ${x%%p}
  Replace,         // ${x/p/r}
  ReplaceAll,      // ${x//p/r}
  ReplaceStart,    // ${x/#p/r}
  ReplaceEnd,      // ${x/%p/r}
  Substring,       // ${x:off}  ${x:off:len}
  ArrayFilter,     // ${x:#p}   remove matching elements ((M) keeps them)
  ArrayDifference, // ${x:|arr}
  ArrayIntersect,  // ${x:*arr}
  ArrayZip,        // ${x:^arr} ${x:^^arr}
  Modifiers,       // ${x:h:t}
};

// Parsed form of the text between ${ and }.
struct ParamExpr {
  vec<ParamFlagSpec> flags;
  bool length = false;      // ${#x}
  bool is_set_test = false; // ${+x}
  bool split = false;       // ${=x}  (== turns SH_WORD_SPLIT off)
  bool no_split = false;
  bool glob_subst = false;  // ${~x}  (~~ off)
  bool no_glob_subst = false;
  bool rc_expand = false;   // ${^x}  (^^ off)
  bool no_rc_expand = false;

  // Either a parameter name ("PATH", "1", "@", "#"...) or, for nested
  // expansions ${${x}...} and ${"$(cmd)"...}, the raw inner word.
  str name;
  bool nested = false;
  // Subscripts in order: ${a[1][2]}.
  vec<str> subscripts;

  ParamOp op = ParamOp::None;
  bool colon = false;  // the ':' variant of - = ? +
  bool longest = false; // ## %% ::= ^^ //
  str operand;          // word / pattern / offset
  str replacement;      // Replace* right side, Substring length
  vec<Modifier> modifiers;

  bool operator==(const ParamExpr&) const = default;
};

s::expected<ParamExpr, ParseError> parse_param_expr(strv inner);

// A parameter expansion result before word splitting. Arrays keep element
// boundaries ("$@" semantics); scalars are a single element.
struct ExpansionValue {
  vec<str> values;
  bool is_array = false;
  // Not-yet-resolved split and glob behaviour requested by flags.
  bool split = false;
  bool glob_subst = false;
  bool rc_expand = false;

  bool operator==(const ExpansionValue&) const = default;
};

struct ParamExpander {
  explicit ParamExpander(Shell& shell) : mShell(shell) {}

  // Applies, in zsh's order (see "Rules" in zshexpn(1)): nested expansion,
  // subscripting, (P), the :- := :? :+ ops, # % / : ops and modifiers,
  // (@)/double-quote joining, (j) join, (s)/(f)/(z)/(0) split, (o)(O)(n)
  // (i)(a) sorting, (u) unique, (q)(Q)(V)(D)(e)(%) transformations, (l)(r)
  // padding, (L)(U)(C) case, (k)(v)(t)(#)(c)(w)(W) and ${#...} length.
  ExpansionValue expand(const ParamExpr& expr, bool in_double_quotes);

private:
  Shell& mShell;
};

}  // namespace myzsh

#endif  // PARAM_EXPAND_H
