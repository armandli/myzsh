#ifndef BUILTIN_H
#define BUILTIN_H

#include <array>
#include <cstdint>
#include <expected>
#include <functional>

#include <util/aliases.h>

namespace myzsh {

struct Shell;

// zsh-style builtin option parsing (Src/builtin.c execbuiltin). optstr
// lists the accepted letters; a letter followed by ':' takes an argument
// (attached "-u2" or separate "-u 2"), '::' an optional attached argument,
// '+' accepts the "+x" form as well. Parsing stops at the first non-option,
// at "--", or at "-" alone (which is consumed).
struct BuiltinArgs {
  // 0 unset, 1 given as -x, 2 given as +x (zsh's OPT_MINUS / OPT_PLUS).
  s::array<s::uint8_t, 128> flags{};
  umap<char, str> values;
  vec<str> operands;

  bool has(char letter) const { return flags[letter] != 0; }
  bool minus(char letter) const { return flags[letter] == 1; }
  bool plus(char letter) const { return flags[letter] == 2; }
  opt<str> value(char letter) const;
};

// Error text "bad option: -X" / "argument expected: -X".
s::expected<BuiltinArgs, str> parse_builtin_args(const vec<str>& args,
                                                 strv optstr);

// args[0] is the builtin's name as invoked; output goes to fd 1/2 directly
// so redirections applied by the executor take effect.
using BuiltinFunction = s::function<int(Shell&, const vec<str>& args)>;

enum BuiltinFlag : s::uint32_t {
  BF_NONE = 0,
  // POSIX special builtin (POSIX_BUILTINS: assignments persist, errors exit).
  BF_SPECIAL = 1u << 0,
  // typeset family: name=value arguments are parsed as assignments.
  BF_ASSIGN = 1u << 1,
  // precommand modifier handled by the executor: builtin command exec
  // noglob nocorrect - .
  BF_PREFIX = 1u << 2,
  BF_DISABLED = 1u << 3,
};

struct Builtin {
  str name;
  BuiltinFunction function;
  s::uint32_t flags = BF_NONE;
};

struct BuiltinTable {
  void add(Builtin builtin);
  opt<s::reference_wrapper<const Builtin>> find(strv name) const;
  void set_disabled(strv name, bool disabled);
  vec<str> names() const;
  // Registers every builtin declared below.
  void register_all();

private:
  umap<str, Builtin> mBuiltins;
};

// --- control.cpp
int bi_colon(Shell& shell, const vec<str>& args);    // : true
int bi_false(Shell& shell, const vec<str>& args);
int bi_break(Shell& shell, const vec<str>& args);    // break continue
int bi_return(Shell& shell, const vec<str>& args);
int bi_exit(Shell& shell, const vec<str>& args);     // exit bye logout
int bi_shift(Shell& shell, const vec<str>& args);
int bi_eval(Shell& shell, const vec<str>& args);
int bi_source(Shell& shell, const vec<str>& args);   // source .
// --- dirs.cpp
int bi_cd(Shell& shell, const vec<str>& args);       // cd chdir pushd
int bi_popd(Shell& shell, const vec<str>& args);
int bi_dirs(Shell& shell, const vec<str>& args);
int bi_pwd(Shell& shell, const vec<str>& args);
// --- output.cpp
int bi_echo(Shell& shell, const vec<str>& args);
int bi_print(Shell& shell, const vec<str>& args);    // print pushln
int bi_printf(Shell& shell, const vec<str>& args);
// --- input.cpp
int bi_read(Shell& shell, const vec<str>& args);
int bi_getln(Shell& shell, const vec<str>& args);
int bi_vared(Shell& shell, const vec<str>& args);
// --- vars.cpp
int bi_typeset(Shell& shell, const vec<str>& args);  // typeset declare
                                                     // local export readonly
                                                     // integer float
int bi_unset(Shell& shell, const vec<str>& args);
int bi_set(Shell& shell, const vec<str>& args);
int bi_let(Shell& shell, const vec<str>& args);
// --- options_cmds.cpp
int bi_setopt(Shell& shell, const vec<str>& args);   // setopt unsetopt
int bi_emulate(Shell& shell, const vec<str>& args);
// --- alias_cmds.cpp
int bi_alias(Shell& shell, const vec<str>& args);
int bi_unalias(Shell& shell, const vec<str>& args);
// --- hash_cmds.cpp
int bi_hash(Shell& shell, const vec<str>& args);
int bi_unhash(Shell& shell, const vec<str>& args);   // unhash unfunction
int bi_rehash(Shell& shell, const vec<str>& args);
// --- lookup.cpp
int bi_whence(Shell& shell, const vec<str>& args);   // whence which where
                                                     // type
// --- function_cmds.cpp
int bi_functions(Shell& shell, const vec<str>& args); // functions autoload
// --- job_cmds.cpp
int bi_jobs(Shell& shell, const vec<str>& args);
int bi_fg(Shell& shell, const vec<str>& args);       // fg bg %job
int bi_wait(Shell& shell, const vec<str>& args);
int bi_disown(Shell& shell, const vec<str>& args);
int bi_kill(Shell& shell, const vec<str>& args);
int bi_suspend(Shell& shell, const vec<str>& args);
// --- trap_cmd.cpp
int bi_trap(Shell& shell, const vec<str>& args);
// --- test_cmd.cpp
int bi_test(Shell& shell, const vec<str>& args);     // test [
// --- getopts.cpp
int bi_getopts(Shell& shell, const vec<str>& args);
// --- history_cmds.cpp
int bi_fc(Shell& shell, const vec<str>& args);       // fc history r
// --- misc.cpp
int bi_umask(Shell& shell, const vec<str>& args);
int bi_ulimit(Shell& shell, const vec<str>& args);
int bi_limit(Shell& shell, const vec<str>& args);    // limit unlimit
int bi_times(Shell& shell, const vec<str>& args);
int bi_enable(Shell& shell, const vec<str>& args);   // enable disable
int bi_zmodload(Shell& shell, const vec<str>& args);
int bi_ttyctl(Shell& shell, const vec<str>& args);
// --- zle_cmds.cpp
int bi_bindkey(Shell& shell, const vec<str>& args);
int bi_zle(Shell& shell, const vec<str>& args);

}  // namespace myzsh

#endif  // BUILTIN_H
