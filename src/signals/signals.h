#ifndef SIGNALS_H
#define SIGNALS_H

#include <csignal>

#include <util/aliases.h>

namespace myzsh {

// Pseudo-signals that can be trapped (Src/signals.h): numbers beyond the
// real signal range.
constexpr int SIGEXIT = 0;
constexpr int SIGZERR = NSIG;
constexpr int SIGDEBUG = NSIG + 1;
constexpr int SIGNAL_LIMIT = NSIG + 2;

// "INT", "SIGINT", "int", "2", "EXIT", "ZERR", "ERR", "DEBUG".
opt<int> signal_number(strv name);
// Name without "SIG" ("INT"), or "EXIT"/"ZERR"/"DEBUG".
str signal_name(int number);
// All names in signal-number order, for `kill -l` and $signals.
vec<str> signal_names();

enum class TrapKind : int {
  Default, // no trap
  Ignore,  // trap '' SIG
  Code,    // trap 'code' SIG
  Function // TRAPxxx() { ... }
};

struct Trap {
  TrapKind kind = TrapKind::Default;
  str code;
  // Function scope level that set it (LOCAL_TRAPS restores on return).
  int level = 0;
};

struct TrapTable {
  const Trap& get(int signal) const;
  void set(int signal, Trap trap);
  void reset(int signal);
  // Signals with a non-default trap, for `trap` with no arguments.
  vec<int> active() const;
  // `trap` listing: "trap -- 'echo hi' INT".
  vec<str> listing() const;
  // In a subshell, traps (other than ignored ones) are reset.
  void reset_for_subshell();

private:
  umap<int, Trap> mTraps;
};

// Process-wide signal plumbing (Src/signals.c). The C handler only records
// the signal number; Shell::dispatch_signals() runs traps and reaps children
// at safe points (between commands, while waiting).
struct SignalManager {
  // Installs handlers for SIGCHLD SIGINT SIGQUIT SIGTERM SIGHUP SIGWINCH
  // SIGALRM SIGPIPE, ignores SIGTSTP/SIGTTIN/SIGTTOU when interactive with
  // job control.
  static void install(bool interactive, bool job_control);
  // Called after fork() in the child: restore default dispositions for
  // everything not ignored by a trap.
  static void reset_in_child();
  static void catch_signal(int signal);
  static void default_signal(int signal);
  static void ignore_signal(int signal);

  // Take and clear the set of signals received since the last call.
  static vec<int> take_pending();
  static bool interrupted();
  static void clear_interrupt();

  static void block_children();
  static void unblock_children();
};

}  // namespace myzsh

#endif  // SIGNALS_H
