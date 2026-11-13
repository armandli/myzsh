#include <builtins/builtin.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

int bi_alias(Shell& shell, const vec<str>& args) {
  // TODO(impl): alias [-gmrsL] [+gmrsL] [name[=value]...]: define, or print "name=value"
  // (value quoted when needed); -L prints `alias` commands; unknown name status 1.
  todo("bi_alias");
}

int bi_unalias(Shell& shell, const vec<str>& args) {
  // TODO(impl): unalias [-ams] names: -a removes all; "no such hash table element: X".
  todo("bi_unalias");
}

}  // namespace myzsh
