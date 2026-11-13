#ifndef GLOB_H
#define GLOB_H

#include <functional>

#include <util/aliases.h>
#include <util/qstring.h>

namespace myzsh {

struct GlobOptions {
  bool glob_dots = false;
  bool mark_dirs = false;      // MARK_DIRS: append '/' to directories
  bool numeric_sort = false;   // NUMERIC_GLOB_SORT
  bool extended_glob = false;
  bool ksh_glob = false;
  bool bare_glob_qual = true;  // BARE_GLOB_QUAL: trailing (...) qualifiers
  bool case_glob = true;
  bool glob_star_short = false; // GLOB_STAR_SHORT: ** means **/*
  // Directory relative patterns are resolved against; the results keep the
  // relative spelling of the pattern.
  str cwd = ".";
};

// Parsed glob qualifiers, e.g. *(.om[1,3]) or *(/N) or *(e:'[[ -s $REPLY ]]':)
struct GlobQualifiers {
  // file type: / . @ = p % %b %c * (executable) and permission tests
  // r w x A I E R W X s S t f{spec}, owner U G u{id} g{id},
  // size L[+-]n[kmp], times m a c [Mwhms][+-]n, links l[+-]n
  vec<str> tests;  // each test kept in its source form, '^' negation and
                   // '-' (follow links) toggles included in order
  // Sort: o/O followed by n L l a m c d N.
  str sort_keys;
  bool descending = false;
  // [n] or [n,m] slice after sorting.
  opt<s::pair<int, int>> range;
  bool null_glob = false;   // N
  bool glob_dots = false;   // D
  bool mark_dirs = false;   // M
  bool mark_types = false;  // T
  bool numeric_sort = false; // n
  // Modifiers to apply to each result: (:r) (:t)...
  str modifiers;
  // e:code: and +func: evaluated by the shell with $REPLY set.
  vec<str> eval_code;
};

opt<GlobQualifiers> parse_glob_qualifiers(strv text);

struct Globber {
  using QualifierEval = s::function<bool(strv path, strv code)>;

  explicit Globber(GlobOptions options) : mOptions(s::move(options)) {}

  void set_qualifier_evaluator(QualifierEval evaluate) {
    mEvaluate = s::move(evaluate);
  }

  // Sorted matches; empty when nothing matches (the caller decides between
  // NOMATCH error, NULL_GLOB, or keeping the word). Splits the pattern on
  // '/', expands each component against the directory listing, supports
  // **/ (recursive, not following symlinks) and ***/ (following them), and
  // honours a trailing qualifier list when bare_glob_qual or (#q...) is used.
  vec<str> expand(const QString& pattern);

protected:
  vec<str> apply_qualifiers(vec<str> paths, const GlobQualifiers& qualifiers);

private:
  GlobOptions mOptions;
  QualifierEval mEvaluate;
};

}  // namespace myzsh

#endif  // GLOB_H
