#ifndef COMMAND_HASH_H
#define COMMAND_HASH_H

#include <util/aliases.h>

namespace myzsh {

// First executable regular file named `name` in the directories of path.
// An empty directory entry means the current directory.
opt<str> search_path(strv name, const vec<str>& path);

// The command hash table behind `hash`, `rehash`, `unhash -m` and
// HASH_CMDS / HASH_DIRS (Src/hashtable.c cmdnamtab).
struct CommandHash {
  // Cached lookup, falling back to search_path and remembering the result.
  opt<str> find(strv name, const vec<str>& path);
  // hash name=path
  void add(strv name, strv full_path);
  bool remove(strv name);
  void clear();
  // HASH_DIRS / rehash: add every executable in every path directory.
  void fill(const vec<str>& path);
  // Sorted "name=path" pairs for `hash` output.
  vec<s::pair<str, str>> entries() const;

private:
  umap<str, str> mTable;
};

}  // namespace myzsh

#endif  // COMMAND_HASH_H
