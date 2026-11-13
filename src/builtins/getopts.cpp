#include <builtins/builtin.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

int bi_getopts(Shell& shell, const vec<str>& args) {
  // TODO(impl): getopts optstring name [args]: POSIX getopts with OPTIND/OPTARG; leading ':' in
  // optstring is silent mode; '+' options are accepted as in zsh (name gets "+x").
  todo("bi_getopts");
}

}  // namespace myzsh
