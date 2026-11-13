#include <util/strings.h>

#include <util/todo.h>

namespace myzsh {

vec<str> split_fields(strv text, strv ifs) {
  // TODO(impl): walk text; skip leading IFS whitespace; a non-whitespace IFS
  // char (plus adjacent IFS whitespace) terminates one field. Keep empty
  // fields between adjacent non-whitespace delimiters and after a trailing
  // one (see zsh's spacesplit() in Src/utils.c).
  todo("split_fields");
}

vec<str> split_on(strv text, strv separator, bool keep_empty) {
  // TODO(impl): split on every occurrence of the full separator string.
  // An empty separator splits into characters.
  todo("split_on");
}

str join(const vec<str>& words, strv separator) {
  // TODO(impl)
  todo("join");
}

bool is_identifier(strv name) {
  // TODO(impl): [A-Za-z_][A-Za-z0-9_]*, non-empty.
  todo("is_identifier");
}

bool is_ifs_whitespace(char c) {
  // TODO(impl): space, tab, newline (and NUL, which zsh treats as such).
  todo("is_ifs_whitespace");
}

str to_lower(strv text) {
  // TODO(impl)
  todo("to_lower");
}

str to_upper(strv text) {
  // TODO(impl)
  todo("to_upper");
}

str capitalize_words(strv text) {
  // TODO(impl)
  todo("capitalize_words");
}

str quote(strv text, QuoteStyle style) {
  // TODO(impl): mirror quotestring() in Src/utils.c. Backslash style escapes
  // every shell metacharacter and renders control chars as $'\t'.
  todo("quote");
}

str unquote(strv text) {
  // TODO(impl): handle '...', "...", $'...' and backslash.
  todo("unquote");
}

EscapeResult interpret_escapes(strv text, bool echo_style) {
  // TODO(impl): see getkeystring() in Src/utils.c.
  todo("interpret_escapes");
}

opt<s::int64_t> parse_decimal(strv text) {
  // TODO(impl): optional sign, digits only, nullopt on junk or overflow.
  todo("parse_decimal");
}

}  // namespace myzsh
