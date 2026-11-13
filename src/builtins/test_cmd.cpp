#include <builtins/builtin.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

int bi_test(Shell& shell, const vec<str>& args) {
  // TODO(impl): test / [ ... ]: CondEvaluator::evaluate_test; `[` requires a final "]"
  // ("[: ']' expected").
  todo("bi_test");
}

}  // namespace myzsh
