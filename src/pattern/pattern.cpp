#include <pattern/pattern.h>

#include <util/todo.h>

namespace myzsh {

// TODO(impl): compiled representation, e.g. a vector of nodes (Literal,
// AnyChar, Star, CharClass, NumRange, Group/Alternation, Repeat(#/##),
// Exclude(~), Not(^), FlagChange) matched by a backtracking matcher as in
// Src/pattern.c, or a Thompson NFA where back-references are not needed.
struct Pattern::Node {};

s::expected<Pattern, str> Pattern::compile(const QString& source,
                                           const PatternOptions& options)
{
  // TODO(impl): return an error string ("bad pattern: X") on syntax errors
  // such as an unterminated '[' or '('.
  todo("Pattern::compile");
}

s::expected<Pattern, str> Pattern::compile(strv source,
                                           const PatternOptions& options)
{
  return compile(QString(source), options);
}

bool Pattern::matches(strv text) const {
  // TODO(impl)
  todo("Pattern::matches");
}

opt<MatchResult> Pattern::match(strv text) const {
  // TODO(impl)
  todo("Pattern::match");
}

opt<MatchResult> Pattern::match_prefix(strv text, bool longest) const {
  // TODO(impl)
  todo("Pattern::match_prefix");
}

opt<MatchResult> Pattern::match_suffix(strv text, bool longest) const {
  // TODO(impl)
  todo("Pattern::match_suffix");
}

opt<MatchResult> Pattern::search(strv text, size_t from, bool longest) const {
  // TODO(impl)
  todo("Pattern::search");
}

bool Pattern::is_literal() const {
  // TODO(impl)
  todo("Pattern::is_literal");
}

bool has_glob_chars(const QString& source, const PatternOptions& options) {
  // TODO(impl): * ? [ always; ( | < when not sh_glob; ^ ~ # with
  // extended_glob.
  todo("has_glob_chars");
}

}  // namespace myzsh
