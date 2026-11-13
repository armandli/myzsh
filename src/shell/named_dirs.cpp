#include <shell/named_dirs.h>

#include <util/todo.h>

namespace myzsh {

void NamedDirTable::add(strv name, strv path) {
  // TODO(impl)
  todo("NamedDirTable::add");
}

bool NamedDirTable::remove(strv name) {
  // TODO(impl)
  todo("NamedDirTable::remove");
}

opt<str> NamedDirTable::find(strv name) const {
  // TODO(impl)
  todo("NamedDirTable::find");
}

opt<str> NamedDirTable::user_home(strv user) {
  // TODO(impl): getpwnam().
  todo("NamedDirTable::user_home");
}

opt<NamedDirTable::Abbreviation> NamedDirTable::abbreviate(strv path,
                                                           strv home) const
{
  // TODO(impl): prefer the longest match; on equal length prefer a name
  // over home.
  todo("NamedDirTable::abbreviate");
}

vec<s::pair<str, str>> NamedDirTable::entries() const {
  // TODO(impl)
  todo("NamedDirTable::entries");
}

}  // namespace myzsh
