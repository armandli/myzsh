#include <builtins/builtin.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

int bi_typeset(Shell& shell, const vec<str>& args) {
  // TODO(impl): typeset/declare/local/export/readonly/integer/float
  // [+-AaEFfghHilprtuUxZ...] [-L n] [-R n] [-Z n] [-i base] [-T SCALAR array [sep]] [name[=value]...]
  // No names: list matching parameters (-p prints re-runnable commands:
  // "typeset -i n=42", "typeset -a a=( one two )", "typeset -A h=( [k]=v )").
  // In a function, creates locals unless -g. -f lists functions. +x removes a flag.
  // export implies -gx; readonly -r; integer -i; float -E; local = typeset in a function.
  // TYPESET_SILENT; KSH_TYPESET; name=( ... ) array assignment form.
  todo("bi_typeset");
}

int bi_unset(Shell& shell, const vec<str>& args) {
  // TODO(impl): unset [-fmv] names: -f functions, -m patterns. Unsetting a readonly is an error.
  todo("bi_unset");
}

int bi_set(Shell& shell, const vec<str>& args) {
  // TODO(impl): set [+-options] [-o opt] [+o] [-A|+A name values] [--] args
  // No args: list all parameters "name=value" (quoted). -o alone: print option states.
  // +o: print "set -o" commands. -s sorts positional args. -- or - sets positionals.
  todo("bi_set");
}

int bi_let(Shell& shell, const vec<str>& args) {
  // TODO(impl): let exprs...: evaluate each; status 0 if the last is non-zero.
  todo("bi_let");
}

}  // namespace myzsh
