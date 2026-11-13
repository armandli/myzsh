#include <expand/word.h>

#include <util/todo.h>

namespace myzsh {

s::expected<vec<WordPart>, ParseError> parse_word(strv raw) {
  // TODO(impl): one left-to-right scan. Bare $name takes identifier chars,
  // or one of the specials ? $ ! # * @ - 0-9 _ (only one digit unbraced).
  // In zsh, an unbraced "$a[1]" includes the subscript. Inside double
  // quotes only $ ` \ " are special (and \ only before $ ` " \ newline).
  // Adjacent literal characters are merged into one part.
  todo("parse_word");
}

}  // namespace myzsh
