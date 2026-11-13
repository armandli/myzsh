#ifndef PARSER_H
#define PARSER_H

#include <expected>

#include <parse/ast.h>
#include <parse/lexer.h>
#include <util/error.h>

namespace myzsh {

// Recursive-descent parser following Src/parse.c (par_list, par_sublist,
// par_pline, par_cmd, par_for, par_case, par_if, par_while, par_repeat,
// par_subsh, par_funcdef, par_time, par_dinbrack, par_simple, par_redir,
// par_cond_*). Accepts zsh's alternative forms as well:
//   if [[ x ]] { ... } elif ... { ... } else { ... }
//   for i (a b c) cmd        foreach i (a b) ... end
//   while cond { ... }       repeat 3 cmd
//   function f g { ... }     f() cmd
struct Parser {
  explicit Parser(Lexer& lexer) : mLexer(lexer) {}

  // Parses all remaining input before anything runs: -c and eval.
  s::expected<ast::List, ParseError> parse_program();
  // Parses the next complete list up to and including its terminating
  // newline, and reads nothing beyond it, so the caller can run it before
  // more input is consumed. The incremental entry point for script files,
  // source, stdin and interactive lines (Shell::run_input). nullopt at EOF.
  // On an error the rest of the current line is skipped, so an interactive
  // caller can carry on with the next line.
  s::expected<opt<ast::List>, ParseError> parse_line();

protected:
  ast::List parse_list(bool stop_at_newline);
  ast::Sublist parse_sublist();
  ast::Pipeline parse_pipeline();
  opt<ast::Command> parse_command();
  ast::SimpleCommand parse_simple_command();
  opt<ast::Redirect> parse_redirect();
  uptr<ast::CondNode> parse_cond_or();
  uptr<ast::CondNode> parse_cond_and();
  uptr<ast::CondNode> parse_cond_primary();

private:
  Lexer& mLexer;
};

// Parses a whole string; used by eval, -c, `functions` round trips, tests.
s::expected<ast::List, ParseError> parse(strv source);

}  // namespace myzsh

#endif  // PARSER_H
