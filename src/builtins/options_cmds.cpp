#include <builtins/builtin.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

int bi_setopt(Shell& shell, const vec<str>& args) {
  // TODO(impl): setopt/unsetopt [+-options] [-m] [name...]: no args lists changed options
  // (OptionState::changed_options). Errors: "no such option: X".
  todo("bi_setopt");
}

int bi_emulate(Shell& shell, const vec<str>& args) {
  // TODO(impl): emulate [-lLR] [ksh|sh|csh|zsh] [-c cmd] [args...]: no args prints the current
  // emulation; -L also sets LOCAL_OPTIONS LOCAL_TRAPS LOCAL_PATTERNS; -c runs cmd
  // in that emulation then restores; -l lists what would change.
  todo("bi_emulate");
}

}  // namespace myzsh
