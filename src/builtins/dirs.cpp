#include <builtins/builtin.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

int bi_cd(Shell& shell, const vec<str>& args) {
  // TODO(impl): cd/chdir/pushd: cd [-qsLP] [dir | old new | +N | -N]. CDPATH search (prints the
  // new dir when found via CDPATH), CDABLE_VARS, AUTO_PUSHD, PUSHD_IGNORE_DUPS,
  // PUSHD_SILENT, PUSHD_TO_HOME, PUSHD_MINUS, CHASE_LINKS / -P, POSIX_CD.
  // Updates PWD/OLDPWD, runs chpwd and chpwd_functions.
  // Errors: "no such file or directory: X", "not a directory: X", "string not in pwd: X".
  todo("bi_cd");
}

int bi_popd(Shell& shell, const vec<str>& args) {
  // TODO(impl): popd [-q] [+N | -N]: remove entry, cd to the new top. "directory stack empty".
  todo("bi_popd");
}

int bi_dirs(Shell& shell, const vec<str>& args) {
  // TODO(impl): dirs [-c] [-l] [-p] [-v] [dir...]: print stack (with ~ abbreviation unless -l);
  // with arguments replace the stack.
  todo("bi_dirs");
}

int bi_pwd(Shell& shell, const vec<str>& args) {
  // TODO(impl): pwd [-rLP]: $PWD, or the physical path with -r/-P or CHASE_LINKS.
  todo("bi_pwd");
}

}  // namespace myzsh
