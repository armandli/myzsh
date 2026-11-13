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
  // TODO(impl): for -c and eval only: the entire string is parsed before
  // running anything (script files, source and stdin go through run_input,
  // one list at a time; TODO.md §2.3.9); a ShellError aborts the rest with
  // status 1 (unless interactive, where it aborts the current line).
  todo("Shell::run_string");
}

int Shell::run_input(InputSource& input, strv name, InputKind kind) {
  // TODO(impl): Lexer over `input` with the alias expander set; loop
  // Parser::parse_line() (zsh's loop() in Src/init.c): on error report it
  // and recover per `kind` (see shell.h); on nullopt return status;
  // otherwise
  // input.sync_position(), execute, dispatch_signals(), stop when
  // flow.exiting or flow.returning (return ends a sourced file). Aliases
  // take effect from the next list because the lexer consults the alias
  // table as it reads.
  todo("Shell::run_input");
}

int Shell::run_file(strv path, const vec<str>* arguments) {
  // TODO(impl): search $path for names without '/' when not found
  // (PATH_SCRIPT / `.` semantics); SOURCE_TRACE; return statement ends the
  // file. Open the file as an FdInput and hand it to
  // run_input(InputKind::Sourced): it is parsed and run one list at a time,
  // not read whole; a parse error in it makes source return 126.
  todo("Shell::run_file");
}

int Shell::run_interactive() {
  // TODO(impl): loop { preprompt(); read a line with ZLE (or from stdin
  // with PS1 printed when not a tty); on EOF honour IGNORE_EOF and
  // CHECK_JOBS; history-expand (HIST_VERIFY: if changed, offer the
  // expanded line for editing instead of running it); add to history and
  // append it to HISTFILE for INC_APPEND_HISTORY/SHARE_HISTORY; run the
  // preexec hooks; run_input over a StringInput of the line, reading more
  // with PS2 while the lexer reports incomplete() }.
  todo("Shell::run_interactive");
}

void Shell::preprompt() {
  // TODO(impl): see the comment in shell.h. SHARE_HISTORY also reads
  // entries other shells appended (History::read_new).
  todo("Shell::preprompt");
}

void Shell::run_sched_events() {
  // TODO(impl): pop and run_string() every event with time <= now; $? is
  // preserved.
  todo("Shell::run_sched_events");
}

void Shell::check_mail() {
  // TODO(impl): MAILCHECK (default 60) seconds since last_mail_check;
  // MAILPATH entries are "file?message" with $_ set to the file, else
  // MAIL alone. Compare mtime against atime and the last check.
  todo("Shell::check_mail");
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
