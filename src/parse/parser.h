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

  // Parses all remaining input.
  s::expected<ast::List, ParseError> parse_program();
  // Parses up to the end of the next complete line (interactive mode, and
  // `eval` of a multi-line string runs line by line). nullopt at EOF.
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
