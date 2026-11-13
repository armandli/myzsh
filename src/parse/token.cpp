#include <parse/token.h>

#include <util/todo.h>

namespace myzsh {

strv token_type_name(TokenType type) {
  // TODO(impl): human-readable name used in "parse error near `X'".
  todo("token_type_name");
}

bool is_redirection(TokenType type) {
  // TODO(impl): OutAng .. TrInAng (zsh's IS_REDIROP).
  todo("is_redirection");
}

opt<TokenType> reserved_word(strv text) {
  // TODO(impl): table of reswds[] from Src/hashtable.c, including the
  // typeset family (declare export float integer local readonly typeset).
  todo("reserved_word");
}

}  // namespace myzsh
