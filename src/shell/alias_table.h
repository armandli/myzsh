#ifndef ALIAS_TABLE_H
#define ALIAS_TABLE_H

#include <util/aliases.h>

namespace myzsh {

enum class AliasKind : int {
  Regular, // expanded in command position
  Global,  // alias -g: expanded anywhere
  Suffix,  // alias -s: "file.ext" in command position runs "text file.ext"
};

struct Alias {
  str name;
  str text;
  AliasKind kind = AliasKind::Regular;
  bool disabled = false; // disable -a
  // Expansion in progress, to stop recursion ("alias ls='ls -F'").
  bool in_use = false;
};

// Src/hashtable.c aliastab / sufaliastab.
struct AliasTable {
  void define(strv name, strv text, AliasKind kind);
  bool remove(strv name, AliasKind kind);
  opt<s::reference_wrapper<const Alias>> find(strv name,
                                              AliasKind kind) const;
  // Expansion lookup used by the lexer: a regular alias only in command
  // position; global aliases anywhere; skips disabled and in-use aliases.
  opt<str> expansion(strv word, bool command_position) const;
  // `alias` listing lines: "name=text", quoted as needed; -g/-s prefixes
  // with "alias -g ..." when listing with -L.
  vec<str> listing(AliasKind kind, bool as_commands) const;
  void set_disabled(strv name, AliasKind kind, bool disabled);
  // Expansion ending in a blank makes the next word alias-eligible too.
  static bool ends_with_blank(strv text);

private:
  umap<str, Alias> mRegular;
  umap<str, Alias> mSuffix;
};

}  // namespace myzsh

#endif  // ALIAS_TABLE_H
