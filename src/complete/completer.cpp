#include <complete/completer.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

CompletionContext Completer::classify(const CompletionRequest& request) const {
  // TODO(impl): lex the buffer up to the cursor; command position after
  // start, ; | & && || ( and precommand modifiers.
  todo("Completer::classify");
}

CompletionResult Completer::complete(const CompletionRequest& request) {
  // TODO(impl)
  todo("Completer::complete");
}

vec<str> Completer::complete_commands(strv prefix) {
  // TODO(impl)
  todo("Completer::complete_commands");
}

vec<str> Completer::complete_files(strv prefix, bool directories_only) {
  // TODO(impl): hidden files only when the prefix starts with '.' (or
  // GLOB_DOTS); keep the user's directory spelling, expand ~ for lookup.
  todo("Completer::complete_files");
}

vec<str> Completer::complete_parameters(strv prefix) {
  // TODO(impl)
  todo("Completer::complete_parameters");
}

vec<str> Completer::complete_options(strv prefix) {
  // TODO(impl)
  todo("Completer::complete_options");
}

vec<str> Completer::complete_users(strv prefix) {
  // TODO(impl)
  todo("Completer::complete_users");
}

str longest_common_prefix(const vec<str>& words) {
  // TODO(impl)
  todo("longest_common_prefix");
}

vec<str> format_columns(const vec<str>& words, int columns, bool rows_first) {
  // TODO(impl): column width = longest word + 2.
  todo("format_columns");
}

}  // namespace myzsh
