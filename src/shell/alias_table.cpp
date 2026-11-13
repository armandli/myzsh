#include <shell/alias_table.h>

#include <util/todo.h>

namespace myzsh {

void AliasTable::define(strv name, strv text, AliasKind kind) {
  // TODO(impl): Regular and Global share the namespace (mRegular).
  todo("AliasTable::define");
}

bool AliasTable::remove(strv name, AliasKind kind) {
  // TODO(impl)
  todo("AliasTable::remove");
}

opt<s::reference_wrapper<const Alias>> AliasTable::find(strv name,
                                                        AliasKind kind) const
{
  // TODO(impl)
  todo("AliasTable::find");
}

opt<str> AliasTable::expansion(strv word, bool command_position) const {
  // TODO(impl)
  todo("AliasTable::expansion");
}

vec<str> AliasTable::listing(AliasKind kind, bool as_commands) const {
  // TODO(impl): sorted by name; values quoted with QuoteStyle::MinimalSingle.
  todo("AliasTable::listing");
}

void AliasTable::set_disabled(strv name, AliasKind kind, bool disabled) {
  // TODO(impl)
  todo("AliasTable::set_disabled");
}

bool AliasTable::ends_with_blank(strv text) {
  // TODO(impl)
  todo("AliasTable::ends_with_blank");
}

}  // namespace myzsh
