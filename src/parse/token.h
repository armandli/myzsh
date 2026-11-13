#ifndef TOKEN_H
#define TOKEN_H

#include <util/aliases.h>

namespace myzsh {

// Mirrors enum lextok in Src/zsh.h.
enum class TokenType : int {
  Null,
  Seper,         // ;  (or newline when parsing a list)
  Newline,
  Semi,
  DSemi,         // ;;
  Amper,         // &
  InPar,         // (
  OutPar,        // )
  DBar,          // ||
  DAmper,        // &&
  OutAng,        // >
  OutAngBang,    // >| >!
  DOutAng,       // >>
  DOutAngBang,   // >>| >>!
  InAng,         // <
  InOutAng,      // <>
  DInAng,        // <<
  DInAngDash,    // <<-
  InAngAmp,      // <&
  OutAngAmp,     // >&
  AmpOutAng,     // &>
  OutAngAmpBang, // >&| >&!
  DOutAngAmp,    // >>&  &>>
  DOutAngAmpBang,// >>&| >>&!
  TrInAng,       // <<<
  Bar,           // |
  BarAmp,        // |&
  InOutPar,      // ()
  DInPar,        // ((
  DOutPar,       // ))
  AmperBang,     // &| &!
  SemiAmp,       // ;&
  SemiBar,       // ;|
  DOutBrack,     // ]]
  String,        // any word
  EnvString,     // name=value  (assignment word)
  EnvArray,      // name=(      (start of array assignment)
  EndInput,
  LexErr,
  // Reserved words: only recognised in command position.
  Bang,          // !
  DInBrack,      // [[
  InBrace,       // {
  OutBrace,      // }
  Case,
  Coproc,
  DoLoop,        // do
  Done,
  Elif,
  Else,
  ZEnd,          // end
  Esac,
  Fi,
  For,
  Foreach,
  Func,          // function
  If,
  NoCorrect,
  Repeat,
  Select,
  Then,
  Time,
  Until,
  While,
  Typeset,       // declare export float integer local readonly typeset
};

struct Token {
  TokenType type = TokenType::Null;
  // Raw source text with quotes intact for String/EnvString/EnvArray;
  // the operator spelling otherwise.
  str text;
  // Explicit fd before a redirection operator ("2>"), -1 if none.
  int fd = -1;
  int line = 1;

  bool operator==(const Token&) const = default;
};

strv token_type_name(TokenType type);
bool is_redirection(TokenType type);
// Reserved word for text, if any ("if" -> If, "{" -> InBrace).
opt<TokenType> reserved_word(strv text);

}  // namespace myzsh

#endif  // TOKEN_H
