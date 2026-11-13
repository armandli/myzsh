#include <zle/display.h>

#include <util/todo.h>

namespace myzsh {

ScreenLayout layout_line(strv prompt, size_t prompt_width, strv buffer,
                         size_t cursor, int columns, size_t rprompt_width)
{
  // TODO(impl): a cursor exactly at the end of a full row moves to the
  // start of the next row.
  todo("layout_line");
}

str render_diff(const ScreenLayout& previous, const ScreenLayout& next) {
  // TODO(impl)
  todo("render_diff");
}

}  // namespace myzsh
