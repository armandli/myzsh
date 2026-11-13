#ifndef LEXER_H
#define LEXER_H

#include <functional>

#include <parse/ast.h>
#include <parse/token.h>
#include <util/aliases.h>

namespace myzsh {

// Splits source text into tokens (Src/lex.c). Context the parser must supply,
// because zsh's lexing is context-sensitive:
//  - command position: reserved words and aliases are recognised only there;
//  - cond mode: inside [[ ]], < > ( ) are condition operators, not redirects;
//  - case pattern mode: '(' and '|' are pattern syntax.
// The lexer tracks two of these itself, as zsh's does: after "[[" it is in
// cond mode until "]]", and after "((" (in command position or after
// "for") it returns the whole expression as one String up to the matching
// "))", followed by DOutPar. In "for ((a;b;c))" the expression String
// contains the semicolons and the parser splits it.
struct Lexer {
  // Returns the replacement text for an alias, or nullopt. The bool says
  // whether the word is in command position (global aliases apply anywhere).
  using AliasExpander = s::function<opt<str>(strv word, bool command_position)>;

  explicit Lexer(strv source);

  // Throws nothing; lexical errors come back as TokenType::LexErr with the
  // message in Token::text.
  Token next();
  // Look ahead without consuming.
  const Token& peek();

  void set_command_position(bool value) { mCommandPosition = value; }
  bool command_position() const { return mCommandPosition; }
  void set_cond_mode(bool value) { mCondMode = value; }
  void set_case_pattern_mode(bool value) { mCasePatternMode = value; }
  void set_alias_expander(AliasExpander expander) {
    mAliasExpander = s::move(expander);
  }
  // Ignore unquoted "#..." comments (INTERACTIVE_COMMENTS; always on for
  // non-interactive shells).
  void set_interactive_comments(bool value) { mComments = value; }

  // The parser saw <<, <<- or <<<-style redirect: the body is read from the
  // line after the next newline token, up to the (unquoted) terminator.
  void queue_heredoc(sptr<ast::HereDoc> target, strv terminator,
                     bool strip_tabs);

  int line() const { return mLine; }
  bool at_end() const;
  // True if input ended inside quotes, $( ), ${ }, or a here-doc.
  bool incomplete() const { return mIncomplete; }

protected:
  // Scans one word, honouring '...', "...", $'...', \x, `...`, $(...),
  // $((...)), ${...}, and glob/brace parentheses, and returns the raw text.
  str scan_word();
  void read_pending_heredocs();

private:
  str mSource;
  size_t mPos = 0;
  int mLine = 1;
  bool mCommandPosition = true;
  bool mCondMode = false;
  bool mCasePatternMode = false;
  bool mComments = true;
  bool mIncomplete = false;
  opt<Token> mPeeked;
  AliasExpander mAliasExpander;

  struct PendingHereDoc {
    sptr<ast::HereDoc> target;
    str terminator;
    bool strip_tabs;
  };
  vec<PendingHereDoc> mPendingHereDocs;
};

// Convenience for tests and history word splitting: tokenises everything with
// default context handling.
vec<Token> tokenize(strv source);

}  // namespace myzsh

#endif  // LEXER_H
