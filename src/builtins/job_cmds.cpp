#include <builtins/builtin.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

int bi_jobs(Shell& shell, const vec<str>& args) {
  // TODO(impl): jobs [-dlprsZ] [%job...]: JobTable::format; -p pgids only; -r running; -s stopped;
  // -Z sets the process title.
  todo("bi_jobs");
}

int bi_fg(Shell& shell, const vec<str>& args) {
  // TODO(impl): fg/bg [%job...] (and a bare `%job`): SIGCONT the job's process group; fg gives
  // it the terminal and waits. Errors: "no current job", "no such job: X",
  // "no job control in this shell.".
  todo("bi_fg");
}

int bi_wait(Shell& shell, const vec<str>& args) {
  // TODO(impl): wait [%job | pid ...]: no args waits for all; status of the last waited job;
  // unknown pid gives 127 ("pid N is not a child of this shell").
  todo("bi_wait");
}

int bi_disown(Shell& shell, const vec<str>& args) {
  // TODO(impl): disown [%job...]: remove from the job table (no HUP on exit).
  todo("bi_disown");
}

int bi_kill(Shell& shell, const vec<str>& args) {
  // TODO(impl): kill [-s sig | -n num | -sig] pid|%job ... / kill -l [sig...] / kill -L
  // "kill: illegal pid: X"; signal names via signal_number().
  todo("bi_kill");
}

int bi_suspend(Shell& shell, const vec<str>& args) {
  // TODO(impl): suspend [-f]: SIGTSTP self; refuses for a login shell without -f.
  todo("bi_suspend");
}

}  // namespace myzsh
