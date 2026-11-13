#include <builtins/builtin.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

int bi_functions(Shell& shell, const vec<str>& args) {
  // TODO(impl): functions [-UkmtTuWz] [-M mathfn] [+-x N] [names]: list function definitions
  // (function_to_text); +-t / -T tracing; -u/-U autoload flags. autoload [-UXktz] names
  // is functions -u; autoload -X inside a stub loads and runs it; -w zwc files are out of scope.
  todo("bi_functions");
}

}  // namespace myzsh
