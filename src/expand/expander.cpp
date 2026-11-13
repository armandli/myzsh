#include <expand/expander.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

vec<str> Expander::expand_words(const vec<ast::Word>& words) {
  // TODO(impl): expand_word for each; RC_EXPAND_PARAM / ${^x} produce the
  // cartesian product with surrounding text instead of joining.
  todo("Expander::expand_words");
}

vec<str> Expander::expand_word(strv raw) {
  // TODO(impl): prefork -> expand_braces (unless IGNORE_BRACES) ->
  // expand_tilde -> glob_or_literal.
  todo("Expander::expand_word");
}

str Expander::expand_assignment(strv raw) {
  // TODO(impl)
  todo("Expander::expand_assignment");
}

str Expander::expand_scalar(strv raw) {
  // TODO(impl)
  todo("Expander::expand_scalar");
}

QString Expander::expand_pattern(strv raw) {
  // TODO(impl): text produced by an unquoted $x is pattern-active only with
  // GLOB_SUBST or ${~x}; otherwise it is marked quoted.
  todo("Expander::expand_pattern");
}

str Expander::expand_heredoc(strv body) {
  // TODO(impl)
  todo("Expander::expand_heredoc");
}

str Expander::expand_string(strv text) {
  // TODO(impl)
  todo("Expander::expand_string");
}

str Expander::command_substitution(strv command) {
  // TODO(impl): parse the text, fork, child runs it with stdout to a pipe
  // (ZSH_SUBSHELL incremented, traps reset), parent reads all output and
  // waits; $? becomes the child's status.
  todo("Expander::command_substitution");
}

str Expander::process_substitution(strv command, bool for_reading,
                                   bool temp_file)
{
  // TODO(impl): the fd must stay open until the command using it finishes;
  // register it with the executor for cleanup.
  todo("Expander::process_substitution");
}

vec<QString> Expander::prefork(strv raw, bool split_allowed) {
  // TODO(impl): parse_word, then expand each part, concatenating quoted and
  // unquoted pieces; array results in unquoted context or "$@"/"${a[@]}"
  // start new fields.
  todo("Expander::prefork");
}

vec<str> Expander::glob_or_literal(const QString& word) {
  // TODO(impl): if has_glob_chars and GLOB is set, run Globber; on no match
  // honour NULL_GLOB / NOMATCH (ShellError "no matches found: WORD") /
  // CSH_NULL_GLOB; otherwise the word with quotes removed.
  todo("Expander::glob_or_literal");
}

}  // namespace myzsh
