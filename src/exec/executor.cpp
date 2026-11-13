#include <exec/executor.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

int Executor::execute(const ast::List& list) {
  // TODO(impl): for each item: run sublist sync or async; after each run
  // pending traps, check ERR_EXIT / ERR_RETURN (not in conditions, not for
  // negated pipelines, not on the left of && ||), and stop on
  // break/continue/return/exit or an error. DEBUG trap runs before each
  // command (after with DEBUG_BEFORE_CMD off).
  todo("Executor::execute");
}

int Executor::execute_sublist(const ast::Sublist& sublist, bool background) {
  // TODO(impl): && / || short-circuit; ! inverts; coproc sets up the
  // coprocess pipes used by read -p / print -p and <&p >&p.
  todo("Executor::execute_sublist");
}

int Executor::execute_pipeline(const ast::Pipeline& pipeline,
                               bool background)
{
  // TODO(impl): every element but the last forks; in zsh the last element
  // runs in the current shell (so `echo x | read v` sets v). One process
  // group per job when MONITOR is set; register the job; wait in the
  // foreground (giving the terminal to the job), set $pipestatus, status is
  // the last command's (or the rightmost failure with PIPE_FAIL).
  todo("Executor::execute_pipeline");
}

int Executor::execute_command(const ast::Command& command) {
  // TODO(impl): std::visit over CommandNode; compound commands apply
  // command.redirects around their body.
  todo("Executor::execute_command");
}

int Executor::call_function(strv name, const vec<str>& args) {
  // TODO(impl): resolve autoload stubs; push positional params and param
  // scope; set $0 to the function name (FUNCTION_ARGZERO); funcstack;
  // "maximum nested function level reached; increase FUNCNEST?".
  todo("Executor::call_function");
}

void Executor::exec_external(const vec<str>& argv) {
  // TODO(impl): "command not found: X" exits 127, "permission denied: X"
  // exits 126.
  todo("Executor::exec_external");
}

int Executor::execute_simple(const ast::SimpleCommand& command,
                             const vec<ast::Redirect>& extra_redirects)
{
  // TODO(impl): order (execcmd in Src/exec.c):
  //  1. expand words; precommand modifiers - noglob nocorrect command
  //     builtin exec (and their options -p -v -V -c -l -a) shape the rest;
  //  2. no words: assignments set shell params (status from last cmd subst)
  //     and redirections are applied then undone (or run NULLCMD/READNULLCMD);
  //  3. resolution: function > builtin > hashed/PATH command; disabled ones
  //     skipped; suffix aliases; AUTO_CD for a directory name;
  //  4. builtins/functions: prefix assignments are temporary (except for
  //     special builtins with POSIX_BUILTINS), redirections saved/restored;
  //  5. external: fork unless this is the last command of a subshell
  //     (exec optimisation); assignments go to the child's environment;
  //  6. XTRACE prints PS4-expanded "+zsh:LINE> cmd args" to stderr.
  todo("Executor::execute_simple");
}

int Executor::execute_subshell(const ast::Subshell& node) {
  // TODO(impl): fork; child increments ZSH_SUBSHELL, runs body, exits.
  todo("Executor::execute_subshell");
}

int Executor::execute_brace(const ast::BraceGroup& node) {
  // TODO(impl)
  todo("Executor::execute_brace");
}

int Executor::execute_try(const ast::Try& node) {
  // TODO(impl): always-block runs even after errors/break/return; inside it
  // $TRY_BLOCK_ERROR is 1 if the try block had an error; after it the
  // error state is restored unless TRY_BLOCK_ERROR was set to 0.
  todo("Executor::execute_try");
}

int Executor::execute_for(const ast::For& node) {
  // TODO(impl): multiple variables take consecutive words per iteration;
  // missing ones are set to empty.
  todo("Executor::execute_for");
}

int Executor::execute_arith_for(const ast::ArithFor& node) {
  // TODO(impl): empty condition is true.
  todo("Executor::execute_arith_for");
}

int Executor::execute_select(const ast::Select& node) {
  // TODO(impl): print menu to stderr in columns, PS3 prompt, read REPLY;
  // EOF ends the loop.
  todo("Executor::execute_select");
}

int Executor::execute_while(const ast::While& node) {
  // TODO(impl): status 0 if the body never ran.
  todo("Executor::execute_while");
}

int Executor::execute_repeat(const ast::Repeat& node) {
  // TODO(impl): count is an arithmetic expression evaluated once.
  todo("Executor::execute_repeat");
}

int Executor::execute_if(const ast::If& node) {
  // TODO(impl): status 0 if no branch ran.
  todo("Executor::execute_if");
}

int Executor::execute_case(const ast::Case& node) {
  // TODO(impl): patterns via Expander::expand_pattern; ;& runs the next body
  // without testing; ;| continues testing the following patterns.
  todo("Executor::execute_case");
}

int Executor::execute_funcdef(const ast::FuncDef& node) {
  // TODO(impl): define each name (expanded); anonymous functions run now.
  todo("Executor::execute_funcdef");
}

int Executor::execute_cond(const ast::Cond& node) {
  // TODO(impl)
  todo("Executor::execute_cond");
}

int Executor::execute_arith(const ast::Arith& node) {
  // TODO(impl): status 0 if the value is non-zero, 1 if zero, 2 on error.
  todo("Executor::execute_arith");
}

int Executor::execute_time(const ast::Time& node) {
  // TODO(impl): getrusage(RUSAGE_CHILDREN) before/after; print with
  // TIMEFMT ("%J  %U user %S system %P cpu %*E total"). Bare `time` prints
  // the shell's own times.
  todo("Executor::execute_time");
}

void Executor::apply_assignments(const vec<ast::Assignment>& assignments) {
  // TODO(impl): scalars via expand_assignment, arrays via expand_words,
  // subscripted assignment, +=; assoc "a=(k v ...)" and "a=([k]=v ...)".
  todo("Executor::apply_assignments");
}

pid_t Executor::fork_child(pid_t process_group, bool foreground) {
  // TODO(impl)
  todo("Executor::fork_child");
}

}  // namespace myzsh
