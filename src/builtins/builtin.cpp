#include <builtins/builtin.h>

#include <util/todo.h>

namespace myzsh {

opt<str> BuiltinArgs::value(char letter) const {
  auto found = values.find(letter);
  if (found == values.end()) {
    return s::nullopt;
  }
  return found->second;
}

s::expected<BuiltinArgs, str> parse_builtin_args(const vec<str>& args,
                                                 strv optstr)
{
  // TODO(impl): args[0] is the command name and is skipped.
  todo("parse_builtin_args");
}

void BuiltinTable::add(Builtin builtin) {
  // TODO(impl)
  todo("BuiltinTable::add");
}

opt<s::reference_wrapper<const Builtin>> BuiltinTable::find(
    strv name) const
{
  // TODO(impl): disabled builtins are not found.
  todo("BuiltinTable::find");
}

void BuiltinTable::set_disabled(strv name, bool disabled) {
  // TODO(impl)
  todo("BuiltinTable::set_disabled");
}

vec<str> BuiltinTable::names() const {
  // TODO(impl)
  todo("BuiltinTable::names");
}

void BuiltinTable::register_all() {
  // TODO(impl): one add() per name; several names share a function and
  // dispatch on args[0] (e.g. "unsetopt" -> bi_setopt). Prefix entries
  // (builtin command exec noglob nocorrect - ) get BF_PREFIX and a null
  // function. Special builtins: . : break continue eval exec exit export
  // readonly return set shift source times trap unset.
  todo("BuiltinTable::register_all");
}

}  // namespace myzsh
