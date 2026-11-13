#ifndef COMPLETER_H
#define COMPLETER_H

#include <util/aliases.h>

namespace myzsh {

struct Shell;

enum class CompletionContext : int {
  Command,   // first word: commands, builtins, functions, aliases,
             // reserved words, and directories (AUTO_CD)
  Argument,  // files (cd/pushd: directories only)
  Parameter, // after $ or ${
  Tilde,     // ~user and named directories
  Option,    // setopt/unsetopt arguments: option names
  Redirect,  // after > < : files
};

struct CompletionRequest {
  str buffer;
  size_t cursor = 0;
};

struct CompletionResult {
  CompletionContext context = CompletionContext::Argument;
  // Byte range of the word being completed (replaced on insertion).
  size_t word_start = 0;
  size_t word_end = 0;
  vec<str> matches;    // full replacement words, sorted, unique
  // Longest common prefix of matches (what a first TAB inserts).
  str common_prefix;
  // Directories get a trailing '/', unique non-directories a space
  // (AUTO_REMOVE_SLASH etc. are handled by the editor).
  vec<str> suffixes;
};

// A self-contained completer in the spirit of compctl's defaults. The full
// compsys (compinit, _arguments, zstyle) is a later milestone; see TODO.md.
struct Completer {
  explicit Completer(Shell& shell) : mShell(shell) {}

  CompletionContext classify(const CompletionRequest& request) const;
  CompletionResult complete(const CompletionRequest& request);

  vec<str> complete_commands(strv prefix);
  vec<str> complete_files(strv prefix, bool directories_only);
  vec<str> complete_parameters(strv prefix);
  vec<str> complete_options(strv prefix);
  vec<str> complete_users(strv prefix);

private:
  Shell& mShell;
};

str longest_common_prefix(const vec<str>& words);
// Columnar listing as zsh prints completion lists: rows fill first unless
// LIST_ROWS_FIRST is false (then columns first), widths fit `columns`.
vec<str> format_columns(const vec<str>& words, int columns, bool rows_first);

}  // namespace myzsh

#endif  // COMPLETER_H
