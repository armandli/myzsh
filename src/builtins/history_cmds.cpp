#include <builtins/builtin.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

int bi_fc(Shell& shell, const vec<str>& args) {
  // TODO(impl): fc [-e editor] [-LI] [-nlrdDfEim] [first [last]] / fc -s [old=new] [cmd] / r /
  // history (= fc -l) / fc -p/-P push/pop history stacks, fc -R/-W/-A read/write/append
  // $HISTFILE. -l output is "%5d  %s" per entry. Editing writes a temp file, runs
  // $FCEDIT/$EDITOR (default vi) and executes the result.
  todo("bi_fc");
}

}  // namespace myzsh
