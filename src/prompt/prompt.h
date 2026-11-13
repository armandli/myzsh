#ifndef PROMPT_H
#define PROMPT_H

#include <util/aliases.h>

namespace myzsh {

struct Shell;

struct ExpandedPrompt {
  str text;
  // Display width: excludes escape sequences emitted for %B %F etc. and
  // anything inside %{ %}.
  size_t width = 0;
};

// Prompt expansion (Src/prompt.c promptexpand):
//   %% %) %n %m %M %~ %/ %d %c %C %. %N~ %N/ %# %? %h %! %j %L %i %I %l %y
//   %T %t %@ %* %w %W %D %D{strftime} %B %b %U %u %S %s %F{c} %f %K{c} %k
//   %{ %} %G %(x.true.false) with conditions ? # ! / ~ c . C D d j L l S T t
//   v w e _ %N<..< %N>..> %<< %e %N %x %I %_ %^ %v %E
// PROMPT_SUBST: $-expansions and $(cmd) before % codes; PROMPT_BANG: '!' is
// the history number ('!!' a literal '!'); PROMPT_PERCENT enables % codes.
// Colours: names black red green yellow blue magenta cyan white default,
// or 0-255 (38;5;N), or #rrggbb (38;2;r;g;b).
ExpandedPrompt expand_prompt(strv prompt, Shell& shell);

// Width of text as displayed, ignoring ANSI CSI sequences.
size_t display_width(strv text);

}  // namespace myzsh

#endif  // PROMPT_H
