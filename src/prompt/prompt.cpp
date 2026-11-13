#include <prompt/prompt.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

ExpandedPrompt expand_prompt(strv prompt, Shell& shell) {
  // TODO(impl): %~ uses the longest named-directory or $HOME prefix of
  // $PWD; truncation %N<str< applies to the following text up to %<<.
  todo("expand_prompt");
}

size_t display_width(strv text) {
  // TODO(impl): count UTF-8 code points with wcwidth().
  todo("display_width");
}

}  // namespace myzsh
