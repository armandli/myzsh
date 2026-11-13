#include <builtins/builtin.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

int bi_bindkey(Shell& shell, const vec<str>& args) {
  // TODO(impl): bindkey [-lLdDANmrsuvea] [-M keymap] [in-string command]: list keymaps (-l),
  // select emacs (-e) / viins (-v) / vicmd (-a), bind sequences (^X, \e, ^[[A notation
  // via parse_key_sequence), -s string macros, -r unbind, -L output as commands.
  todo("bi_bindkey");
}

int bi_zle(Shell& shell, const vec<str>& args) {
  // TODO(impl): zle [-N widget [function]] [-D widget] [-A old new] [-l [-a]] [-R] [-M msg] [-U str]
  // [-K keymap] [widget args]: user-defined widgets, invoke widgets from widget
  // functions, list widgets. Fails outside of the editor for widget invocation.
  todo("bi_zle");
}

}  // namespace myzsh
