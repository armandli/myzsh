#include <builtins/builtin.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

int bi_trap(Shell& shell, const vec<str>& args) {
  // TODO(impl): trap [code] [signals...] / trap - sigs / trap '' sigs / trap (list) / trap -l
  // A first argument that is a signal number resets those signals. Traps on EXIT
  // inside a function run when the function returns. Setting a trap removes any
  // TRAPxxx function and vice versa.
  todo("bi_trap");
}

}  // namespace myzsh
