#include <builtins/builtin.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

int bi_umask(Shell& shell, const vec<str>& args) {
  // TODO(impl): umask [-S] [mask]: octal or symbolic (u=rwx,g=rx,o=).
  todo("bi_umask");
}

int bi_ulimit(Shell& shell, const vec<str>& args) {
  // TODO(impl): ulimit [-HSa] [-cdflmnstuv...] [limit]: getrlimit/setrlimit, units of 512 bytes or
  // kilobytes as in zsh; "unlimited".
  todo("bi_ulimit");
}

int bi_limit(Shell& shell, const vec<str>& args) {
  // TODO(impl): limit [-hs] [resource [value]] / unlimit [-hs] [resource]: csh-style names
  // (cputime filesize datasize stacksize coredumpsize memoryuse descriptors ...).
  todo("bi_limit");
}

int bi_times(Shell& shell, const vec<str>& args) {
  // TODO(impl): times: shell and children user/system times, "0m0.01s 0m0.02s" lines.
  todo("bi_times");
}

int bi_enable(Shell& shell, const vec<str>& args) {
  // TODO(impl): enable/disable [-afmprs] names: toggles builtins (default), -a aliases,
  // -f functions, -r reserved words, -s suffix aliases, -p patterns.
  todo("bi_enable");
}

int bi_zmodload(Shell& shell, const vec<str>& args) {
  // TODO(impl): zmodload [-dLaiFue] [module]: modules are compiled in; accept known names
  // (zsh/parameter zsh/zutil zsh/complete zsh/zle zsh/datetime zsh/mathfunc ...) as
  // already loaded and fail with "failed to load module: X" otherwise.
  todo("bi_zmodload");
}

int bi_ttyctl(Shell& shell, const vec<str>& args) {
  // TODO(impl): ttyctl [-fu]: freeze/unfreeze saved terminal settings.
  todo("bi_ttyctl");
}

int bi_sched(Shell& shell, const vec<str>& args) {
  // TODO(impl): sched [-o] [+]hh:mm[:ss] cmd / sched [+]seconds cmd adds to
  // Shell::sched_events (kept sorted); sched alone lists
  // "  1 Thu Nov 12 10:00:00 cmd"; sched -N removes item N. Events run from
  // Shell::preprompt and while ZLE is idle (Shell::run_sched_events).
  todo("bi_sched");
}

}  // namespace myzsh
