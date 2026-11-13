#include <builtins/builtin.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

int bi_colon(Shell& shell, const vec<str>& args) {
  // TODO(impl): :, true: return 0 ignoring arguments.
  todo("bi_colon");
}

int bi_false(Shell& shell, const vec<str>& args) {
  // TODO(impl): Return 1.
  todo("bi_false");
}

int bi_break(Shell& shell, const vec<str>& args) {
  // TODO(impl): break [N] / continue [N] (args[0] says which): set flow.breaks / flow.continuing.
  // Errors: "not in while, until, select, or repeat loop" (status 1), N must be > 0.
  todo("bi_break");
}

int bi_return(Shell& shell, const vec<str>& args) {
  // TODO(impl): return [N]: from a function or sourced file set
  // flow.returning and status N (an arithmetic expression, default $?).
  // At the top level of a script it ends the script like exit.
  todo("bi_return");
}

int bi_exit(Shell& shell, const vec<str>& args) {
  // TODO(impl): exit/bye/logout [N]: logout only in login shells ("not login shell").
  // With stopped jobs and CHECK_JOBS warn once ("you have suspended jobs.").
  todo("bi_exit");
}

int bi_shift(Shell& shell, const vec<str>& args) {
  // TODO(impl): shift [-p] [N] [array...]: -p shifts from the end; N > $# is an error
  // ("shift count must be <= $#").
  todo("bi_shift");
}

int bi_eval(Shell& shell, const vec<str>& args) {
  // TODO(impl): eval words...: join with spaces, parse and run in the current shell.
  todo("bi_eval");
}

int bi_source(Shell& shell, const vec<str>& args) {
  // TODO(impl): source/. file [args]: Shell::run_file; `.` searches $path first, source
  // checks the current directory first.
  todo("bi_source");
}

}  // namespace myzsh
