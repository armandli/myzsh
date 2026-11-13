#ifndef REDIRECT_H
#define REDIRECT_H

#include <parse/ast.h>
#include <util/aliases.h>

namespace myzsh {

struct Shell;

// Applies a command's redirections (Src/exec.c addfd/execcmd). For builtins,
// functions and compound commands in the current shell the original fds are
// saved and restored by restore() / the destructor; in a forked child they
// are applied permanently (save = false).
//
// MULTIOS: several output redirections of one fd ("> a > b", "> a | cat")
// create a tee process; several input redirections concatenate.
// NO_CLOBBER: '>' on an existing regular file fails ("file exists: f")
// unless >| or >!. Here-docs and here-strings are written to a temp file or
// pipe. {var}>file allocates an fd >= 10 and stores it in $var.
struct Redirector {
  Redirector(Shell& shell, bool save) : mShell(shell), mSave(save) {}
  ~Redirector();
  Redirector(const Redirector&) = delete;
  Redirector& operator=(const Redirector&) = delete;

  // Throws ShellError on failure (the command is then not run, status 1).
  void apply(const vec<ast::Redirect>& redirects);
  void restore();

protected:
  void redirect_fd(int target_fd, int source_fd);
  int open_target(const ast::Redirect& redirect, strv path);

private:
  Shell& mShell;
  bool mSave;
  // (fd, saved copy or -1 if it was closed before)
  vec<s::pair<int, int>> mSaved;
  vec<int> mMultiosPids;
};

}  // namespace myzsh

#endif  // REDIRECT_H
