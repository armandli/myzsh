#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <sys/types.h>

#include <parse/ast.h>
#include <util/aliases.h>

namespace myzsh {

struct Shell;

// Walks the AST (Src/exec.c execlist/execpline/execcmd, Src/loop.c).
// Status of every construct is returned and also stored in Shell::status.
// break/continue/return/exit are not exceptions: they set counters in
// Shell::flow and every loop/list checks them after each command, as zsh
// does with breaks/contflag/retflag.
struct Executor {
  explicit Executor(Shell& shell) : mShell(shell) {}

  int execute(const ast::List& list);
  int execute_sublist(const ast::Sublist& sublist, bool background);
  int execute_pipeline(const ast::Pipeline& pipeline, bool background);
  int execute_command(const ast::Command& command);

  // Runs a function body with "$@" = args and a new parameter scope,
  // honouring LOCAL_OPTIONS/LOCAL_TRAPS/FUNCNEST, return status.
  int call_function(strv name, const vec<str>& args);

  // Final step for an external command in a (possibly forked) child:
  // execve with the exported environment; ENOEXEC falls back to running
  // the file as a shell script. Never returns.
  [[noreturn]] void exec_external(const vec<str>& argv);

protected:
  int execute_simple(const ast::SimpleCommand& command,
                     const vec<ast::Redirect>& extra_redirects);
  int execute_subshell(const ast::Subshell& node);
  int execute_brace(const ast::BraceGroup& node);
  int execute_try(const ast::Try& node);
  int execute_for(const ast::For& node);
  int execute_arith_for(const ast::ArithFor& node);
  int execute_select(const ast::Select& node);
  int execute_while(const ast::While& node);
  int execute_repeat(const ast::Repeat& node);
  int execute_if(const ast::If& node);
  int execute_case(const ast::Case& node);
  int execute_funcdef(const ast::FuncDef& node);
  int execute_cond(const ast::Cond& node);
  int execute_arith(const ast::Arith& node);
  int execute_time(const ast::Time& node);

  void apply_assignments(const vec<ast::Assignment>& assignments);
  // Forks; in the child resets signals, joins the process group when job
  // control is on, and returns 0; in the parent returns the pid.
  pid_t fork_child(pid_t process_group, bool foreground);

private:
  Shell& mShell;
};

}  // namespace myzsh

#endif  // EXECUTOR_H
