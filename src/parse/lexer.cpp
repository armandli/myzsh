#include <parse/lexer.h>

#include <util/todo.h>

namespace myzsh {

Lexer::Lexer(InputSource& input) : mInput(&input) {}

Lexer::Lexer(strv source)
    : mOwnedInput(s::make_unique<StringInput>(source)),
      mInput(mOwnedInput.get()) {}

int Lexer::next_char() {
  // TODO(impl): mInput->next_char(); a '\n' increments mLine.
  todo("Lexer::next_char");
}

void Lexer::unget(int c) {
  // TODO(impl): END_OF_INPUT is not pushed back; '\n' decrements mLine.
  todo("Lexer::unget");
}

Token Lexer::next() {
  // TODO(impl): see gettok() in Src/lex.c.
  //  - skip blanks and (unquoted) comments; backslash-newline is a join;
  //  - newline -> Newline (and read pending here-doc bodies after it);
  //  - operators by longest match: ; ;; ;& ;| & && &| &! | || |& ( ) (( ))
  //    < > >> <> << <<- <<< <& >& &> >| >! >>| >& >&| >>& &>> ...;
  //  - a leading run of digits immediately before < or > is Token::fd;
  //  - "()" in command position is InOutPar (function definition);
  //  - words: name=value -> EnvString, name=( -> EnvArray, name+=...;
  //  - in command position, reserved_word() and then alias expansion (aliases
  //    are expanded with mInput->push_text(replacement); guard against
  //    recursive expansion of the same alias);
  //  - never read past the end of the current token plus one character of
  //    lookahead (given back with unget): after a Newline token nothing of
  //    the next line has been consumed except pending here-doc bodies;
  //  - "]]" only ends a cond in cond mode.
  todo("Lexer::next");
}

const Token& Lexer::peek() {
  // TODO(impl): cache in mPeeked. Note that context changes made after a
  // peek do not re-lex the cached token.
  todo("Lexer::peek");
}

void Lexer::queue_heredoc(sptr<ast::HereDoc> target, strv terminator,
                          bool strip_tabs)
{
  // TODO(impl): quoted terminator ('EOF', "EOF", \EOF) => target->quoted and
  // the terminator is compared after quote removal.
  todo("Lexer::queue_heredoc");
}

bool Lexer::at_end() {
  // TODO(impl): peek().type == EndInput.
  todo("Lexer::at_end");
}

str Lexer::scan_word() {
  // TODO(impl): set mIncomplete if the input ends inside a quote/nesting.
  todo("Lexer::scan_word");
}

void Lexer::read_pending_heredocs() {
  // TODO(impl): <<- strips leading tabs from body lines and terminator line.
  todo("Lexer::read_pending_heredocs");
}

vec<Token> tokenize(strv source) {
  // TODO(impl): call next() until EndInput or LexErr (inclusive). Reserved
  // words are recognised at the start and after ; & | && || ( { newline.
  todo("tokenize");
}

}  // namespace myzsh
