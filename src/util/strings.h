#ifndef STRINGS_H
#define STRINGS_H

#include <cstdint>

#include <util/aliases.h>

namespace myzsh {

// Default $IFS: space, tab, newline, NUL.
constexpr strv DEFAULT_IFS{" \t\n\0", 4};

// Word splitting as done for ${=var} and SH_WORD_SPLIT. IFS whitespace runs
// collapse and are trimmed at both ends; every non-whitespace IFS character
// delimits exactly one field, so "a::b" with IFS=: gives {a, "", b} and a
// trailing delimiter yields a trailing empty field.
vec<str> split_fields(strv text, strv ifs);

// (s:sep:) splitting: empty fields are dropped unless keep_empty (the (@)
// flag inside double quotes).
vec<str> split_on(strv text, strv separator, bool keep_empty);

str join(const vec<str>& words, strv separator);

bool is_identifier(strv name);
bool is_ifs_whitespace(char c);

str to_lower(strv text);
str to_upper(strv text);
// (C) flag: first letter of each alphanumeric run upper, rest lower.
str capitalize_words(strv text);

// The quoting styles selectable with ${(q)...} and friends.
enum class QuoteStyle : int {
  Backslash,     // (q)
  Single,        // (qq)
  Double,        // (qqq)
  Dollar,        // (qqqq)  $'...'
  MinimalSingle, // (q-)    quote only if needed, prefer '...'
  MinimalDollar, // (q+)    like (q-) but $'...' for unprintables
};

str quote(strv text, QuoteStyle style);
// ${(Q)...}: removes one level of shell quoting.
str unquote(strv text);

struct EscapeResult {
  str text;
  // \c seen: print/echo must stop output and suppress the newline.
  bool stop = false;
};

// Backslash escapes for print/echo: \a \b \c \e \f \n \r \t \v \\ \0NNN
// \xHH \uHHHH \UHHHHHHHH. echo_style selects echo's \NNN octal handling.
EscapeResult interpret_escapes(strv text, bool echo_style);

opt<s::int64_t> parse_decimal(strv text);

}  // namespace myzsh

#endif  // STRINGS_H
