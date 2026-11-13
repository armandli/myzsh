#ifndef SHELL_H
#define SHELL_H

#include <sys/types.h>

#include <builtins/builtin.h>
#include <exec/command_hash.h>
#include <exec/executor.h>
#include <exec/functions.h>
#include <expand/expander.h>
#include <history/history.h>
#include <jobs/jobs.h>
#include <params/param.h>
#include <shell/alias_table.h>
#include <shell/cmdline.h>
#include <shell/named_dirs.h>
#include <shell/options.h>
#include <signals/signals.h>
#include <util/aliases.h>

namespace myzsh {

// Pending non-local control flow, checked after every command
// (zsh's breaks / contflag / retflag / exit_pending).
struct ControlFlow {
  int breaks = 0;          // break N still to unwind
  bool continuing = false; // continue: unwinding to the loop test
  bool returning = false;  // return from function or sourced file
  bool exiting = false;    // exit requested
  int loop_depth = 0;      // for "break: not in while, until, select..."
  int function_depth = 0;
  int source_depth = 0;

  bool interrupting() const {
    return breaks > 0 or continuing or returning or exiting;
  }
};

// All interpreter state. zsh keeps this in globals; one object makes it
// possible to run many independent shells in one test process.
struct Shell {
  Shell();
  ~Shell();
  Shell(const Shell&) = delete;
  Shell& operator=(const Shell&) = delete;

  // Startup (Src/init.c): options from the invocation, emulation,
  // environment import, special params, signal setup, terminal/job control
  // when interactive, and startup files unless NO_RCS:
  //   $ZDOTDIR/.zshenv; login: .zprofile; interactive: .zshrc;
  //   login: .zlogin  (each preceded by its /etc/z* global file unless
  //   NO_GLOBAL_RCS). The constructor does none of this so tests can build a
  //   bare Shell cheaply.
  void init(const Invocation& invocation, char** envp);
  // Minimal setup for tests: options, specials, no environment, no rc files.
  void init_for_test();

  // Entry point used by main(): dispatches to -c / script / stdin /
  // interactive, runs EXIT trap and .zlogout, returns the exit status.
  int run(const Invocation& invocation);

  // Parses and executes source. Parse errors are reported
  // ("NAME:LINE: parse error near `X'") and give status 1. `name` is used
  // for error prefixes and $0-in-scripts handling.
  int run_string(strv source, strv name = "");
  // `source` / `.`: runs a file in the current shell with optional
  // positional parameters. Returns 127 and reports if the file is missing.
  int run_file(strv path, const vec<str>* arguments = nullptr);
  // Read-eval loop on stdin or the terminal (ZLE if USE_ZLE and a tty):
  // prompt PS1/RPS1, PS2 for continuation, history expansion and storage,
  // job notifications, precmd/preexec hooks, IGNORE_EOF, checkjobs on exit.
  int run_interactive();

  // Writes "NAME:LINE: message" to stderr (NAME:cmd:LINE: for builtins).
  void report_error(strv message);
  void report_builtin_error(strv builtin, strv message);

  // Runs trap handlers for signals received since the last call, reaps
  // children and updates the job table; safe points only.
  void dispatch_signals();
  // Runs the trap for a pseudo-signal (SIGEXIT, SIGZERR, SIGDEBUG).
  void run_trap(int signal);
  // Waits for a foreground job, giving it the terminal; returns status.
  int wait_for_job(int job_number);

  // Exits the process after EXIT trap, history save and HUP of jobs.
  [[noreturn]] void exit_shell(int exit_status);

  bool option(Option option) const { return options.is_set(option); }
  Executor executor() { return Executor(*this); }
  Expander expander() { return Expander(*this); }

  OptionState options;
  ParamTable params;
  FunctionTable functions;
  AliasTable aliases;
  NamedDirTable named_dirs;
  CommandHash command_hash;
  BuiltinTable builtins;
  JobTable jobs;
  History history;
  TrapTable traps;
  ControlFlow flow;

  // $1..$N; $0 lives in argzero. Saved/restored around function calls.
  vec<str> positional;
  str argzero = "zsh";
  // Prefix for error messages: script name, or argv[0] basename.
  str error_name = "zsh";
  vec<str> directory_stack;

  int status = 0;              // $?
  vec<int> pipestatus;         // $pipestatus
  pid_t shell_pid = 0;         // $$
  pid_t last_background = 0;   // $!
  int lineno = 1;              // $LINENO
  int subshell_level = 0;      // $ZSH_SUBSHELL
  bool interactive = false;
  // Current command is a condition (if/while test, && || left side) so
  // ERR_EXIT must not fire.
  bool in_condition = false;
  // fds of the coprocess for read -p / print -p, -1 if none.
  int coproc_in = -1;
  int coproc_out = -1;
};

}  // namespace myzsh

#endif  // SHELL_H
