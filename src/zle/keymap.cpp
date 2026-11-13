#include <zle/keymap.h>

#include <util/todo.h>

namespace myzsh {

str parse_key_sequence(strv notation) {
  // TODO(impl): getkeystring() with GETKEYS_BINDKEY in Src/utils.c.
  todo("parse_key_sequence");
}

str format_key_sequence(strv keys) {
  // TODO(impl): bindztrdup() in Src/Zle/zle_keymap.c.
  todo("format_key_sequence");
}

void Keymap::bind(strv keys, KeyBinding binding) {
  // TODO(impl)
  todo("Keymap::bind");
}

bool Keymap::unbind(strv keys) {
  // TODO(impl)
  todo("Keymap::unbind");
}

opt<KeyBinding> Keymap::lookup(strv keys) const {
  // TODO(impl)
  todo("Keymap::lookup");
}

bool Keymap::is_prefix(strv keys) const {
  // TODO(impl)
  todo("Keymap::is_prefix");
}

vec<s::pair<str, KeyBinding>> Keymap::bindings() const {
  // TODO(impl)
  todo("Keymap::bindings");
}

Keymap make_emacs_keymap() {
  // TODO(impl): from Src/Zle/zle_bindings.c emacsbind[] plus the common
  // escape sequences: ^A beginning-of-line, ^E end-of-line, ^F/^B char,
  // \ef/\eb word, ^D delete-char-or-list, ^H/^? backward-delete-char,
  // ^K kill-line, ^U kill-whole-line, ^W backward-kill-word, ^Y yank,
  // \ey yank-pop, ^T transpose-chars, ^P/^N history, ^R/^S isearch, ^_ undo,
  // ^I expand-or-complete, ^J/^M accept-line, ^L clear-screen, ^[[A/B/C/D
  // arrows, \e. insert-last-word, \eq push-line, ^X^U undo ...
  todo("make_emacs_keymap");
}

Keymap make_viins_keymap() {
  // TODO(impl): ^[ vi-cmd-mode, ^H/^? vi-backward-delete-char,
  // ^W vi-backward-kill-word, ^U vi-kill-line, ^I/^J/^M as emacs.
  todo("make_viins_keymap");
}

Keymap make_vicmd_keymap() {
  // TODO(impl): h j k l w b e W B E 0 ^ $ x X i a I A o O d c y p P u r R
  // . f F t T ; , / ? n N G gg ~ J etc. Unbound keys beep (fallback
  // "beep").
  todo("make_vicmd_keymap");
}

KeymapSet::KeymapSet() {
  // Construction is side-effect free; the default keymaps are created by
  // LineEditor::init so tests of unrelated parts do not depend on them.
}

opt<s::reference_wrapper<Keymap>> KeymapSet::get(strv name) {
  // TODO(impl)
  todo("KeymapSet::get");
}

void KeymapSet::create(strv name, Keymap keymap) {
  // TODO(impl)
  todo("KeymapSet::create");
}

bool KeymapSet::remove(strv name) {
  // TODO(impl): the built-in keymaps can't be deleted.
  todo("KeymapSet::remove");
}

bool KeymapSet::link(strv existing, strv alias) {
  // TODO(impl)
  todo("KeymapSet::link");
}

void KeymapSet::select(strv name) {
  // TODO(impl)
  todo("KeymapSet::select");
}

vec<str> KeymapSet::names() const {
  // TODO(impl)
  todo("KeymapSet::names");
}

}  // namespace myzsh
