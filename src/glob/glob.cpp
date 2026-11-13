#include <glob/glob.h>

#include <util/todo.h>

namespace myzsh {

opt<GlobQualifiers> parse_glob_qualifiers(strv text) {
  // TODO(impl): text is the inside of the trailing (...). Return nullopt if
  // it is not a valid qualifier list (then the parens are pattern syntax).
  todo("parse_glob_qualifiers");
}

vec<str> Globber::expand(const QString& pattern) {
  // TODO(impl): see zglob()/scanner() in Src/glob.c. Default sort is by
  // name using the C locale ordering zsh uses for glob results.
  todo("Globber::expand");
}

vec<str> Globber::apply_qualifiers(vec<str> paths,
                                   const GlobQualifiers& qualifiers)
{
  // TODO(impl)
  todo("Globber::apply_qualifiers");
}

}  // namespace myzsh
