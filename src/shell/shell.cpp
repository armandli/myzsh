#include <shell/shell.h>

#include <unistd.h>

#include <params/special.h>
#include <parse/parser.h>
#include <util/todo.h>

namespace myzsh {

Shell::Shell() : shell_pid(getpid()) {
  // Construction must stay cheap and side-effect free: tests create many
  // shells. Everything that touches the process happens in init().
}

Shell::~Shell() = default;

void Shell::init(const Invocation& invocation, char** envp) {
  // TODO(impl): see the comment in shell.h. SHLVL is incremented, PWD is
  // validated against "." (else getcwd), HOME/USERNAME/HOST/PATH defaults.
  todo("Shell::init");
}

void Shell::init_for_test() {
  // TODO(impl): options.reset(Zsh), register builtins, install specials,
  // wire ParamTable::set_arithmetic to ArithEvaluator. Must not read the
  // environment or rc files.
  todo("Shell::init_for_test");
}

int Shell::run(const Invocation& invocation) {
  // TODO(impl)
  todo("Shell::run");
}

int Shell::run_string(strv source, strv name) {
  // TODO(impl): non-interactive scripts are parsed whole (zsh -c parses the
  // entire string before running anything); a ShellError aborts the rest
  // with status 1 (unless interactive, where it aborts the current line).
  todo("Shell::run_string");
}

int Shell::run_file(strv path, const vec<str>* arguments) {
  // TODO(impl): search $path for names without '/' when not found
  // (PATH_SCRIPT / `.` semantics); SOURCE_TRACE; return statement ends the
  // file.
  todo("Shell::run_file");
}

int Shell::run_interactive() {
  // TODO(impl)
  todo("Shell::run_interactive");
}

void Shell::report_error(strv message) {
  // TODO(impl)
  todo("Shell::report_error");
}

void Shell::report_builtin_error(strv builtin, strv message) {
  // TODO(impl)
  todo("Shell::report_builtin_error");
}

void Shell::dispatch_signals() {
  // TODO(impl)
  todo("Shell::dispatch_signals");
}

void Shell::run_trap(int signal) {
  // TODO(impl): $? is preserved across the trap; ZERR doesn't nest.
  todo("Shell::run_trap");
}

int Shell::wait_for_job(int job_number) {
  // TODO(impl)
  todo("Shell::wait_for_job");
}

void Shell::exit_shell(int exit_status) {
  // TODO(impl)
  todo("Shell::exit_shell");
}

}  // namespace myzsh
