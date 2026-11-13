#include <builtins/builtin.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

int bi_whence(Shell& shell, const vec<str>& args) {
  // TODO(impl): whence [-acmpvfsSwx] names / which (= whence -c) / where (= whence -ca) /
  // type (= whence -v). Reports alias, reserved word, function, builtin, hashed or path
  // command. -v: "ls is /bin/ls", "cd is a shell builtin", "f is a shell function",
  // "ll is an alias for ls -l", "if is a reserved word"; -w: "name: kind"; not found:
  // "x not found" and status 1. which/-c prints function definitions.
  todo("bi_whence");
}

}  // namespace myzsh
