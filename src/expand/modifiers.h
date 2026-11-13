#ifndef MODIFIERS_H
#define MODIFIERS_H

#include <expected>

#include <util/aliases.h>

namespace myzsh {

// History/parameter/glob modifiers (modify() in Src/hist.c):
//   :a :A :c :e :h[N] :l :p :P :q :Q :r :s/l/r[/] :& :t[N] :u :x
//   :g (global prefix for s and &), :w (per word), :W:sep:
struct Modifier {
  char op = 0;
  int count = 1;         // :h2, :t2
  bool global = false;   // :gs/a/b/
  bool per_word = false; // :w
  str left;              // :s/left/right/
  str right;
  str separator;         // :W:sep:

  bool operator==(const Modifier&) const = default;
};

// Parses a modifier chain such as ":h:t" or ":gs/x/y/:u". Returns an error
// message for an unknown modifier.
s::expected<vec<Modifier>, str> parse_modifiers(strv text);

// Context needed by the few filesystem-aware modifiers.
struct ModifierContext {
  str pwd;            // :a :A
  vec<str> path;      // :c
  // :s and :& remember the last substitution across calls.
  str last_left;
  str last_right;
};

str apply_modifier(strv value, const Modifier& modifier,
                   ModifierContext& context);
str apply_modifiers(strv value, const vec<Modifier>& modifiers,
                    ModifierContext& context);

}  // namespace myzsh

#endif  // MODIFIERS_H
