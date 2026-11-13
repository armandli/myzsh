#ifndef KEYMAP_H
#define KEYMAP_H

#include <util/aliases.h>

namespace myzsh {

// "^A" -> "\x01", "^[" / "\e" / "\E" -> ESC, "^?" -> DEL, "\M-x" / "\C-x",
// octal "\033", hex "\x1b", anything else literal (bindkey notation).
str parse_key_sequence(strv notation);
// Inverse, as printed by `bindkey`: "^A", "^[[A", "^?", "\M-a"...
str format_key_sequence(strv keys);

struct KeyBinding {
  // Widget name, or the string to push back for `bindkey -s`.
  str target;
  bool is_macro = false;

  bool operator==(const KeyBinding&) const = default;
};

// One keymap (Src/Zle/zle_keymap.c): a map from key sequences to widgets
// with prefix detection so multi-key sequences can be read.
struct Keymap {
  void bind(strv keys, KeyBinding binding);
  bool unbind(strv keys);
  opt<KeyBinding> lookup(strv keys) const;
  // Some longer bound sequence starts with keys (keep reading, subject to
  // $KEYTIMEOUT).
  bool is_prefix(strv keys) const;
  // Sorted (keys, binding) pairs for `bindkey` listing.
  vec<s::pair<str, KeyBinding>> bindings() const;
  // Single bytes with no binding self-insert in insert keymaps.
  str fallback_widget = "self-insert";

private:
  umap<str, KeyBinding> mBindings;
};

Keymap make_emacs_keymap();
Keymap make_viins_keymap();
Keymap make_vicmd_keymap();

// Named keymaps with aliases: emacs viins vicmd isearch command menuselect,
// and "main" linked to emacs (or viins when $EDITOR/$VISUAL contains "vi").
struct KeymapSet {
  KeymapSet();
  opt<s::reference_wrapper<Keymap>> get(strv name);
  void create(strv name, Keymap keymap);
  bool remove(strv name);
  // bindkey -A old new: make `new` an alias for `old`.
  bool link(strv existing, strv alias);
  void select(strv name);
  strv selected() const { return mSelected; }
  vec<str> names() const;

private:
  umap<str, sptr<Keymap>> mKeymaps;
  str mSelected = "main";
};

}  // namespace myzsh

#endif  // KEYMAP_H
