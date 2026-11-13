#ifndef DISPLAY_H
#define DISPLAY_H

#include <util/aliases.h>

namespace myzsh {

// Pure layout of the edit line (Src/Zle/zle_refresh.c) so it can be tested
// without a terminal: where each row starts and where the cursor ends up,
// given the prompt width and terminal columns. Multi-line buffers and wide
// characters are wrapped; control characters display as ^X.
struct ScreenLayout {
  vec<str> rows;      // display text of each screen row (prompt included)
  int cursor_row = 0;
  int cursor_column = 0;
  // Right prompt shown on the first row (fits, and TRANSIENT_RPROMPT not
  // applied yet).
  bool show_rprompt = false;
};

ScreenLayout layout_line(strv prompt, size_t prompt_width, strv buffer,
                         size_t cursor, int columns, size_t rprompt_width);

// Escape sequences to turn the previous screen into the new one with
// minimal output (full redraw is acceptable for a first version).
str render_diff(const ScreenLayout& previous, const ScreenLayout& next);

}  // namespace myzsh

#endif  // DISPLAY_H
