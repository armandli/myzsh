#ifndef NAMED_DIRS_H
#define NAMED_DIRS_H

#include <util/aliases.h>

namespace myzsh {

// Named directories for ~name and %~ (Src/hashnameddir.c): entries from
// `hash -d name=path`, AUTO_NAME_DIRS / CDABLE_VARS parameters, and users'
// home directories looked up on demand.
struct NamedDirTable {
  void add(strv name, strv path);
  bool remove(strv name);
  opt<str> find(strv name) const;
  // Home directory of a user from the passwd database.
  static opt<str> user_home(strv user);

  struct Abbreviation {
    str name;   // "" means $HOME (prints as "~")
    size_t length; // number of characters of path replaced
  };
  // Longest named directory (or home) that is a prefix of path, on a '/'
  // boundary; used by %~ and `dirs`.
  opt<Abbreviation> abbreviate(strv path, strv home) const;
  vec<s::pair<str, str>> entries() const;

private:
  umap<str, str> mDirs;
};

}  // namespace myzsh

#endif  // NAMED_DIRS_H
