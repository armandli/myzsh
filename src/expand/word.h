#ifndef WORD_H
#define WORD_H

#include <expected>

#include <util/aliases.h>
#include <util/error.h>

namespace myzsh {

// Structure of one raw shell word, produced at expansion time from the text
// the lexer kept. Example: a"$b"${c:-x}'d' ->
//   Literal(a) DoubleQuoted[Param(b)] Param(c:-x) SingleQuoted(d)
enum class WordPartKind : int {
  Literal,       // unquoted text (glob chars, ~, = stay special)
  Escaped,       // \c : one literal character
  SingleQuoted,  // '...'
  DollarQuoted,  // $'...'   text is already unescaped
  DoubleQuoted,  // "..."   children hold the parts
  Param,         // $name, $1, $?, ${...}: text is the inside of ${ }
  CommandSubst,  // $(...) or `...`: text is the command
  ArithSubst,    // $((...)) or $[...]: text is the expression
  ProcSubstIn,   // <(...)
  ProcSubstOut,  // >(...)
  ProcSubstFile, // =(...)
};

struct WordPart {
  WordPartKind kind = WordPartKind::Literal;
  str text;
  vec<WordPart> children;
  // Param: written as ${...} (so a following [ or : is part of it) rather
  // than a bare $name.
  bool braced = false;

  bool operator==(const WordPart&) const = default;
};

s::expected<vec<WordPart>, ParseError> parse_word(strv raw);

}  // namespace myzsh

#endif  // WORD_H
