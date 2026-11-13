#include <expand/modifiers.h>

#include <util/todo.h>

namespace myzsh {

s::expected<vec<Modifier>, str> parse_modifiers(strv text) {
  // TODO(impl): any delimiter may follow s (":s,a,b,"); a trailing
  // delimiter is optional; '&' in right means the matched text.
  todo("parse_modifiers");
}

str apply_modifier(strv value, const Modifier& modifier,
                   ModifierContext& context)
{
  // TODO(impl): :h on "file" gives "."; :h on "/" gives "/"; :r only
  // removes an extension in the last path component; :e is empty with no
  // extension; :a resolves . and .. textually, :A also follows symlinks.
  todo("apply_modifier");
}

str apply_modifiers(strv value, const vec<Modifier>& modifiers,
                    ModifierContext& context)
{
  // TODO(impl)
  todo("apply_modifiers");
}

}  // namespace myzsh
