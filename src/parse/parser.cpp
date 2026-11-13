#include <parse/parser.h>

#include <util/todo.h>

namespace myzsh {

s::expected<ast::List, ParseError> Parser::parse_program() {
  // TODO(impl): parse_list until EndInput. Turn an internal error (e.g. a
  // ShellError thrown from below, or a LexErr token) into ParseError with
  // message "parse error near `TOKEN'" and the line; set incomplete when the
  // lexer ran out of input inside a construct.
  todo("Parser::parse_program");
}

s::expected<opt<ast::List>, ParseError> Parser::parse_line() {
  // TODO(impl)
  todo("Parser::parse_line");
}

ast::List Parser::parse_list(bool stop_at_newline) {
  // TODO(impl): sublists separated by ; & &| &! newline. Terminating
  // reserved words (then fi do done esac } ...) end the list.
  todo("Parser::parse_list");
}

ast::Sublist Parser::parse_sublist() {
  // TODO(impl): [!] [coproc] pipeline { (&& | ||) newline* pipeline }
  todo("Parser::parse_sublist");
}

ast::Pipeline Parser::parse_pipeline() {
  // TODO(impl): command { (| | |&) newline* command }
  todo("Parser::parse_pipeline");
}

opt<ast::Command> Parser::parse_command() {
  // TODO(impl): dispatch on the reserved word / token in command position;
  // compound commands may be followed by redirections. "name ()" or
  // "function name" starts a FuncDef; "()" with nothing before it is an
  // anonymous function.
  todo("Parser::parse_command");
}

ast::SimpleCommand Parser::parse_simple_command() {
  // TODO(impl): leading EnvString/EnvArray are assignments; once a word is
  // seen, later name=value words are ordinary arguments (except for the
  // Typeset reserved words, where they remain assignments). Redirections
  // may appear anywhere.
  todo("Parser::parse_simple_command");
}

opt<ast::Redirect> Parser::parse_redirect() {
  // TODO(impl): map operator tokens to RedirType; "<&-"/">&-" -> Close;
  // ">&word" where word is not a number and not "-" means ErrWrite (csh
  // style); "{var}>" where the preceding word was {identifier} sets
  // fd_variable; << and <<- call Lexer::queue_heredoc.
  todo("Parser::parse_redirect");
}

uptr<ast::CondNode> Parser::parse_cond_or() {
  // TODO(impl): [[ ]] grammar: || < && < ! < ( ) / unary / binary / string.
  // The right side of = == != is a pattern; of =~ a regex.
  todo("Parser::parse_cond_or");
}

uptr<ast::CondNode> Parser::parse_cond_and() {
  // TODO(impl)
  todo("Parser::parse_cond_and");
}

uptr<ast::CondNode> Parser::parse_cond_primary() {
  // TODO(impl)
  todo("Parser::parse_cond_primary");
}

s::expected<ast::List, ParseError> parse(strv source) {
  Lexer lexer(source);
  Parser parser(lexer);
  return parser.parse_program();
}

}  // namespace myzsh
