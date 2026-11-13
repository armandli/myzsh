#include <gtest/gtest.h>

#include <support/test_support.h>

namespace myzsh {
namespace {

// Each case runs in a fresh shell (init_for_test) inside an empty temporary
// directory. Expected stdout and status were captured from `zsh -f -c` 5.9;
// test/unit/check_builtin_cases.py re-validates this table against a real
// zsh so the expectations stay honest.
struct BuiltinCase {
  const char* name;
  const char* script;
  const char* out;
  int status;
};

const BuiltinCase CASES[] = {
  // --- : true false
  {"colon", R"x(: ignored args; echo $?)x", R"x(0
)x", 0},
  {"false_status", R"x(false)x", R"x()x", 1},
  // --- echo
  {"echo_basic", R"x(echo a  b)x", R"x(a b
)x", 0},
  {"echo_escapes", R"x(echo 'a\tb' ; echo -E 'a\nb')x", "a\tb\na\\nb\n", 0},
  {"echo_n", R"x(echo -n x; echo y)x", R"x(xy
)x", 0},
  {"echo_dashdash", R"x(echo -- -n)x", R"x(-- -n
)x", 0},
  {"echo_backslash_c", R"x(echo 'a\cb'; echo z)x", R"x(az
)x", 0},
  // --- print
  {"print_raw", R"x(print -r 'a\tb')x", R"x(a\tb
)x", 0},
  {"print_lines", R"x(print -l a b)x", R"x(a
b
)x", 0},
  {"print_columns", R"x(print -C2 1 2 3 4)x", R"x(1  3
2  4
)x", 0},
  {"print_columns_across", R"x(print -a -C2 1 2 3 4)x", R"x(1  2
3  4
)x", 0},
  {"print_sort", R"x(print -o c a b; print -O c a b)x", R"x(a b c
c b a
)x", 0},
  {"print_format", R"x(print -f '%s=%s\n' a 1 b 2)x", R"x(a=1
b=2
)x", 0},
  {"print_to_fd", R"x(print -u2 err 2>&1)x", R"x(err
)x", 0},
  {"print_v", R"x(print -v var hello; echo $var)x", R"x(hello
)x", 0},
  {"print_prompt", R"x(print -P '%%')x", R"x(%
)x", 0},
  {"print_null", R"x(print -N a b | tr '\0' '|'; echo)x", R"x(a|b|
)x", 0},
  {"print_dash_dash", R"x(print -- -x)x", R"x(-x
)x", 0},
  {"print_m", R"x(print -m 'a*' abc bcd ade)x", R"x(abc ade
)x", 0},
  // --- printf
  {"printf_basic", R"x(printf '%s-%d\n' x 3)x", R"x(x-3
)x", 0},
  {"printf_v", R"x(printf -v out '%03d' 7; echo $out)x", R"x(007
)x", 0},
  // --- read
  {"read_fields", R"x(read a b <<< 'one two three'; echo "$a|$b")x",
   R"x(one|two three
)x", 0},
  {"read_reply", R"x(read <<< 'line'; echo $REPLY)x", R"x(line
)x", 0},
  {"read_array", R"x(read -A arr <<< 'x y z'; echo $#arr $arr[2])x", R"x(3 y
)x", 0},
  {"read_raw", R"x(read -r v <<< 'a\b'; print -r -- "$v")x", R"x(a\b
)x", 0},
  {"read_backslash", R"x(read v <<< 'a\b'; echo "$v")x", R"x(ab
)x", 0},
  {"read_eof", R"x(read v < /dev/null; echo $?)x", R"x(1
)x", 0},
  {"read_delim", R"x(read -d : v <<< 'ab:cd'; echo $v)x", R"x(ab
)x", 0},
  {"read_k", R"x(read -u 0 -k 2 v <<< 'abcd'; echo $v)x", R"x(ab
)x", 0},
  {"read_ifs", R"x(IFS=: read a b <<< 'x:y:z'; echo $a $b)x", R"x(x y:z
)x", 0},
  // --- typeset family
  {"typeset_integer", R"x(integer i=2+3; echo $i; typeset -i 16 h=255; echo $h)x",
   R"x(5
16#FF
)x", 0},
  {"typeset_float", R"x(float f=1.5; echo $f; typeset -F 2 g=3.14159; echo $g)x",
   R"x(1.500000000e+00
3.14
)x", 0},
  {"typeset_case", R"x(typeset -u up=abc; typeset -l lo=ABC; echo $up $lo)x",
   R"x(ABC abc
)x", 0},
  {"typeset_justify", R"x(typeset -Z 4 z=7; typeset -L 3 l=abcdef; echo $z $l)x",
   R"x(0007 abc
)x", 0},
  {"typeset_p_scalar", R"x(x=hello; typeset -p x)x", R"x(typeset x=hello
)x", 0},
  {"typeset_p_array", R"x(a=(one 'two words'); typeset -p a)x",
   R"x(typeset -a a=( one 'two words' )
)x", 0},
  {"typeset_p_assoc", R"x(typeset -A h=(k v); typeset -p h)x",
   R"x(typeset -A h=( [k]=v )
)x", 0},
  {"typeset_p_integer", R"x(integer n=42; typeset -p n)x", R"x(typeset -i n=42
)x", 0},
  {"typeset_array_assign", R"x(typeset -a arr=(a b c); echo $#arr)x", R"x(3
)x", 0},
  {"typeset_unique", R"x(typeset -U u=(a b a); echo $u)x", R"x(a b
)x", 0},
  {"typeset_tied", R"x(typeset -T MYP myp; myp=(a b); echo $MYP; MYP=x:y; echo $myp)x",
   R"x(a:b
x y
)x", 0},
  {"local_scope", R"x(x=g; f() { local x=l; echo $x }; f; echo $x)x", R"x(l
g
)x", 0},
  {"typeset_in_function_is_local", R"x(f() { typeset y=1 }; f; echo "[$y]")x",
   R"x([]
)x", 0},
  {"typeset_g", R"x(f() { typeset -g y=1 }; f; echo "[$y]")x", R"x([1]
)x", 0},
  {"export_env", R"x(export EV=1; sh -c 'echo $EV')x", R"x(1
)x", 0},
  {"export_unexport", R"x(export EV=1; typeset +x EV; sh -c 'echo "[$EV]"')x",
   R"x([]
)x", 0},
  {"readonly", R"x(readonly r=1; (r=2) 2>/dev/null; echo $? $r)x", R"x(1 1
)x", 0},
  // --- unset
  {"unset_var", R"x(x=1; unset x; echo "[${x-unset}]")x", R"x([unset]
)x", 0},
  {"unset_function", R"x(f() { :; }; unset -f f; whence f; echo $?)x", R"x(1
)x", 0},
  {"unset_element", R"x(typeset -A h=(a 1 b 2); unset 'h[a]'; echo ${(k)h})x",
   R"x(b
)x", 0},
  {"unset_m", R"x(foo1=1 foo2=2; unset -m 'foo*'; echo "[$foo1$foo2]")x", R"x([]
)x", 0},
  // --- set
  {"set_positionals", R"x(set -- a b c; echo $# $2)x", R"x(3 b
)x", 0},
  {"set_A", R"x(set -A arr x y; echo $arr[2])x", R"x(y
)x", 0},
  {"set_option_letter",
   R"x({ set -x; [[ $- == *x* ]] && v=on; set +x } 2>/dev/null; echo $v; [[ $- == *x* ]] || echo off)x",
   R"x(on
off
)x", 0},
  {"set_o_name", R"x(set -o extendedglob; [[ -o extendedglob ]] && echo on)x", R"x(on
)x", 0},
  // --- setopt / unsetopt / emulate
  {"setopt_list", R"x(setopt extendedglob; setopt | grep extendedglob)x",
   R"x(extendedglob
)x", 0},
  {"setopt_unknown", R"x(setopt nosuchopt 2>/dev/null; echo $?)x", R"x(1
)x", 0},
  {"unsetopt", R"x(unsetopt glob; echo *)x", R"x(*
)x", 0},
  {"emulate_print", R"x(emulate)x", R"x(zsh
)x", 0},
  {"emulate_c", R"x(emulate sh -c 'x="a b"; for i in $x; do echo $i; done'; emulate)x",
   R"x(a
b
zsh
)x", 0},
  {"emulate_local", R"x(f() { emulate -L sh; [[ -o shwordsplit ]] && echo in }; f; [[ -o shwordsplit ]] || echo out)x",
   R"x(in
out
)x", 0},
  // --- alias
  {"alias_define_use", "alias hi='echo hello'\neval hi", R"x(hello
)x", 0},
  // zsh always defines run-help and which-command.
  {"alias_list", R"x(alias a=b ll='ls -l'; alias)x", R"x(a=b
ll='ls -l'
run-help=man
which-command=whence
)x", 0},
  {"alias_global", "alias -g UP='| tr a-z A-Z'\neval 'echo abc UP'", R"x(ABC
)x", 0},
  {"alias_show_one", R"x(alias ll='ls -l'; alias ll)x", R"x(ll='ls -l'
)x", 0},
  {"alias_missing", R"x(alias nope; echo $?)x", R"x(1
)x", 0},
  {"unalias", "alias x=y\nunalias x\nalias x; echo $?", R"x(1
)x", 0},
  {"alias_L", R"x(alias -g G=grep; alias -L G)x", R"x(alias -g G=grep
)x", 0},
  // --- hash / whence family
  {"hash_add", R"x(hash myls=/bin/ls; hash | grep myls)x", R"x(myls=/bin/ls
)x", 0},
  {"hash_d", R"x(hash -d proj=/tmp; echo ~proj)x", R"x(/tmp
)x", 0},
  {"whence_kinds", R"x(alias ll=ls; f() { }; whence ll f echo if)x", R"x(ls
f
echo
if
)x", 0},
  {"whence_v", R"x(alias ll='ls -l'; f() { }; whence -v ll f echo if)x",
   R"x(ll is an alias for ls -l
f is a shell function from zsh
echo is a shell builtin
if is a reserved word
)x", 0},
  {"whence_w", R"x(f() { }; whence -w f echo if nosuch)x", R"x(f: function
echo: builtin
if: reserved
nosuch: none
)x", 1},
  {"type_not_found", R"x(type nosuchcmd_xyz)x", R"x(nosuchcmd_xyz not found
)x", 1},
  {"which_function", R"x(f() { echo x }; which f)x", "f () {\n\techo x\n}\n", 0},
  {"where_builtin", R"x(where echo | head -1)x", R"x(echo: shell built-in command
)x", 0},
  // --- functions / autoload
  {"functions_list", R"x(f() { echo x }; functions f)x", "f () {\n\techo x\n}\n", 0},
  {"functions_missing", R"x(functions nosuch; echo $?)x", R"x(1
)x", 0},
  {"unfunction", R"x(f() { }; unfunction f; whence f; echo $?)x", R"x(1
)x", 0},
  {"autoload_fpath", "mkdir fns; echo 'echo autoloaded $1' > fns/af\nfpath=($PWD/fns); autoload af; af x",
   R"x(autoloaded x
)x", 0},
  // --- directories
  {"cd_pwd", R"x(mkdir d; cd d; print -r -- ${PWD:t}; cd ..; print -r -- ${PWD:t:h})x",
   R"x(d
.
)x", 0},
  {"cd_dash", R"x(mkdir d; cd d; cd - >/dev/null; [[ $PWD == $OLDPWD:h ]] && echo back)x",
   R"x(back
)x", 0},
  {"cd_missing", R"x(cd /nonexistent_dir_xyz 2>/dev/null; echo $?)x", R"x(1
)x", 0},
  {"cd_substitute", R"x(mkdir -p a1/x a2/x; cd a1/x; cd a1 a2; print -r -- ${PWD:h:t})x",
   R"x(a2
)x", 0},
  {"pushd_popd", R"x(mkdir a b; pushd a >/dev/null; pushd ../b >/dev/null; echo ${#dirstack} ${dirstack[1]:t}; popd >/dev/null; print -r -- ${PWD:t})x",
   R"x(2 a
a
)x", 0},
  {"popd_empty", R"x(popd 2>/dev/null; echo $?)x", R"x(1
)x", 0},
  {"pwd_builtin", R"x([[ $(pwd) == $PWD ]] && echo same)x", R"x(same
)x", 0},
  // --- control
  {"shift", R"x(set -- a b c d; shift; echo $1; shift 2; echo $1)x", R"x(b
d
)x", 0},
  {"shift_too_many", R"x(set -- a; shift 3 2>/dev/null; echo $? $1)x", R"x(1 a
)x", 0},
  {"shift_array", R"x(arr=(1 2 3); shift arr; echo $arr)x", R"x(2 3
)x", 0},
  {"eval", R"x(cmd='echo $((1+2))'; eval $cmd; eval 'x=5'; echo $x)x", R"x(3
5
)x", 0},
  {"source_args", "echo 'echo $# $1; return 4; echo no' > s.zsh\nsource ./s.zsh a b; echo $?",
   R"x(2 a
4
)x", 0},
  {"dot_missing", R"x(. ./missing.zsh 2>/dev/null; echo $?)x", R"x(127
)x", 0},
  {"return_status", R"x(f() { return 7 }; f; echo $?)x", R"x(7
)x", 0},
  {"exit_status", R"x(exit 3)x", R"x()x", 3},
  {"exit_in_subshell", R"x((exit 5); echo $?)x", R"x(5
)x", 0},
  // A misplaced break is fatal to a non-interactive shell.
  {"break_outside_loop", R"x(break 2>/dev/null; echo $?)x", R"x()x", 1},
  // --- getopts
  {"getopts", R"x(set -- -a -b val rest; while getopts ab: o; do echo $o $OPTARG; done; echo $OPTIND)x",
   R"x(a
b val
4
)x", 0},
  {"getopts_unknown", R"x(set -- -z; getopts a o 2>/dev/null; echo $o)x", R"x(?
)x", 0},
  {"getopts_silent", R"x(set -- -z; getopts :a o; echo $o $OPTARG)x", R"x(? z
)x", 0},
  // --- let / test
  {"let", R"x(let 'x=3*4' 'y = x + 1'; echo $x $y; let 0; echo $?)x", R"x(12 13
1
)x", 0},
  {"test_builtin", R"x(test 1 -lt 2 && [ a = a ] && [ ! -e /nonexistent ] && echo ok)x",
   R"x(ok
)x", 0},
  {"bracket_needs_close", R"x([ a = a 2>/dev/null; echo $?)x", R"x(2
)x", 0},
  // --- traps / kill
  {"trap_list", R"x(trap 'echo hi' INT; trap)x", R"x(trap -- 'echo hi' INT
)x", 0},
  {"trap_exit", R"x(trap 'echo bye' EXIT; echo main)x", R"x(main
bye
)x", 0},
  {"trap_signal", R"x(trap 'echo got USR1' USR1; kill -USR1 $$; echo after)x",
   R"x(got USR1
after
)x", 0},
  {"trap_reset", R"x(trap 'echo x' INT; trap - INT; trap)x", R"x()x", 0},
  {"trap_function", R"x(TRAPUSR2() { echo fn }; kill -USR2 $$; echo done)x", R"x(fn
done
)x", 0},
  {"kill_l", R"x(kill -l INT; kill -l 15)x", R"x(2
TERM
)x", 0},
  // --- jobs
  {"jobs_listing", R"x(sleep 2 & jobs; kill %1)x", R"x([1]  + running    sleep 2
)x", 0},
  {"wait_status", R"x((exit 6) & wait $!; echo $?)x", R"x(6
)x", 0},
  {"disown", R"x(sleep 2 & disown; jobs; echo done; kill $!)x", R"x(done
)x", 0},
  // --- misc
  {"umask", R"x(umask 027; umask; umask -S)x", R"x(027
u=rwx,g=rx,o=
)x", 0},
  {"enable_disable", R"x(disable echo; whence -w echo; enable echo; whence -w echo)x",
   R"x(echo: command
echo: builtin
)x", 0},
  {"builtin_bypasses_function", R"x(echo() { print fn }; builtin echo real)x",
   R"x(real
)x", 0},
  {"command_bypasses_function", R"x(ls() { print fn }; command ls -d .)x", R"x(.
)x", 0},
  {"command_v", R"x(command -v echo; command -v sh)x", R"x(echo
/bin/sh
)x", 0},
  {"exec_redirect", R"x(exec 3>out; print -u3 hi; exec 3>&-; cat out)x", R"x(hi
)x", 0},
  {"exec_replaces", R"x(exec sh -c 'echo replaced'; echo not)x", R"x(replaced
)x", 0},
  {"noglob", R"x(noglob echo *)x", R"x(*
)x", 0},
  {"zmodload_known", R"x(zmodload zsh/datetime; echo $?)x", R"x(0
)x", 0},
};

void PrintTo(const BuiltinCase& param, s::ostream* out) {
  *out << param.name;
}

class BuiltinTest : public ::testing::TestWithParam<BuiltinCase> {};

TEST_P(BuiltinTest, MatchesZsh) {
  const BuiltinCase& param = GetParam();
  test::TempDir dir;
  test::ScopedCwd cwd(dir.path());
  auto shell = test::make_shell();
  shell->params.set_scalar("PATH", "/bin:/usr/bin");
  test::Captured result = test::run(*shell, param.script);
  EXPECT_EQ(result.out, param.out) << "script: " << param.script
                                   << "\nstderr: " << result.err;
  EXPECT_EQ(result.status, param.status) << "script: " << param.script;
}

INSTANTIATE_TEST_SUITE_P(Zsh, BuiltinTest, ::testing::ValuesIn(CASES),
                         [](const auto& info) {
                           return str(info.param.name);
                         });

}  // namespace
}  // namespace myzsh
