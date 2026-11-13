#ifndef PATTERN_H
#define PATTERN_H

#include <expected>

#include <util/aliases.h>
#include <util/qstring.h>

namespace myzsh {

struct PatternOptions {
  bool extended_glob = false; // ^ ~ # ## (#flags)
  bool ksh_glob = false;      // @( ) *( ) +( ) ?( ) !( )
  bool sh_glob = false;       // parentheses and <> are not special
  bool case_insensitive = false;
  // Filename mode: '*' '?' '[...]' never match '/', and a leading '.' must
  // be matched explicitly unless glob_dots.
  bool file_mode = false;
  bool glob_dots = false;
};

struct MatchSpan {
  size_t start = 0;
  size_t end = 0;

  bool operator==(const MatchSpan&) const = default;
};

struct MatchResult {
  MatchSpan span;
  // (#b) back-references: $match / $mbegin / $mend. nullopt for groups that
  // did not participate.
  vec<opt<MatchSpan>> captures;
};

// A compiled zsh pattern (Src/pattern.c). Syntax:
//   * ? [abc] [^abc] [!abc] [a-z] [[:alpha:]] <n-m> (a|b)
//   extended_glob: ^pat  a~b  x#  x##  (#i) (#I) (#l) (#b) (#B) (#m) (#M)
//                  (#aN) approximate  (#s) (#e) anchors  (#q...) ignored
//   ksh_glob:      @(a|b) *(a|b) +(a|b) ?(a|b) !(a|b)
// Quoted characters (per QString) always match literally.
struct Pattern {
  static s::expected<Pattern, str> compile(const QString& source,
                                           const PatternOptions& options);
  static s::expected<Pattern, str> compile(strv source,
                                           const PatternOptions& options);

  // Whole-string match.
  bool matches(strv text) const;
  // Whole-string match with (#b) captures.
  opt<MatchResult> match(strv text) const;
  // ${x#p} / ${x##p}: match anchored at the start.
  opt<MatchResult> match_prefix(strv text, bool longest) const;
  // ${x%p} / ${x%%p}: match anchored at the end.
  opt<MatchResult> match_suffix(strv text, bool longest) const;
  // ${x/p/r}: leftmost match at or after `from`; longest unless the (S)
  // flag asks for shortest.
  opt<MatchResult> search(strv text, size_t from, bool longest) const;

  // True if the pattern contains no special characters: callers can use
  // plain string comparison (and globbing can skip directory scanning).
  bool is_literal() const;

protected:
  struct Node;
  sptr<const Node> mRoot;
  PatternOptions mOptions;
};

// Whether an unquoted character in source would make it a pattern (used to
// decide if a word needs globbing at all).
bool has_glob_chars(const QString& source, const PatternOptions& options);

}  // namespace myzsh

#endif  // PATTERN_H
