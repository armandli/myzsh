#ifndef EXPANDER_H
#define EXPANDER_H

#include <parse/ast.h>
#include <util/aliases.h>
#include <util/qstring.h>

namespace myzsh {

struct Shell;

// Drives the expansion stages of zshexpn(1) for one or more words:
//   1. history expansion   (done on input lines by History, not here)
//   2. alias expansion     (done by the lexer)
//   3. process substitution, parameter expansion, command substitution,
//      arithmetic expansion — left to right in one pass ("prefork")
//   4. brace expansion
//   5. filename expansion (~ and =)
//   6. filename generation (globbing)
// with word splitting of unquoted results (SH_WORD_SPLIT or ${=x}) and
// final quote removal. Empty unquoted words are removed; "" and "$@" with no
// positional parameters follow zsh rules.
struct Expander {
  explicit Expander(Shell& shell) : mShell(shell) {}

  // Command arguments: all stages, may yield zero or many words.
  vec<str> expand_words(const vec<ast::Word>& words);
  vec<str> expand_word(strv raw);

  // Assignment right-hand side: no splitting or globbing (unless
  // GLOB_ASSIGN), tilde after ':' too. Arrays use expand_words.
  str expand_assignment(strv raw);
  // Single word with no splitting/globbing: case subject, here-string,
  // redirection target (globbing a target is an error if ambiguous),
  // for-loop arithmetic, `[[ ]]` operands.
  str expand_scalar(strv raw);
  // Pattern contexts (case patterns, [[ x = pat ]], ${x#pat}): prefork
  // only, keeping quoted-ness so quoted chars match literally.
  QString expand_pattern(strv raw);
  // Unquoted here-document body: $ expansions and backslash-escapes only.
  str expand_heredoc(strv body);
  // String used by ${(e)...}, PROMPT_SUBST and eval-like contexts: prefork
  // expansions on a string treated as if in double quotes.
  str expand_string(strv text);

  // $(...): runs in a subshell, returns output minus trailing newlines.
  str command_substitution(strv command);
  // <(...) and >(...): starts the process and returns /dev/fd/N; =(...)
  // writes output to a temp file and returns its name.
  str process_substitution(strv command, bool for_reading, bool temp_file);

protected:
  // Stage 3 on one word: returns fields that still carry quoting info.
  vec<QString> prefork(strv raw, bool split_allowed);
  vec<str> glob_or_literal(const QString& word);

private:
  Shell& mShell;
};

}  // namespace myzsh

#endif  // EXPANDER_H
