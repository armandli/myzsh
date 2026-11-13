#include <builtins/builtin.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

int bi_echo(Shell& shell, const vec<str>& args) {
  // TODO(impl): echo [-neE] args: escapes on by default unless BSD_ECHO; -n no newline;
  // \c stops output. Unknown options are printed as arguments; no "--" handling.
  todo("bi_echo");
}

int bi_print(Shell& shell, const vec<str>& args) {
  // TODO(impl): print [-abcDilmnNoOpPrRsSz] [-C cols] [-f fmt] [-u fd] [-v name] [-x n] [-X n] args
  // -r raw, -R BSD-echo style, -n no newline, -l one per line, -N NUL-separated,
  // -c/-C columns, -o/-O sort, -i case-insensitive sort, -D ~-abbreviate, -P prompt
  // expansion, -s/-S add to history, -z push onto editor buffer, -u fd, -p coprocess,
  // -v assign instead of printing, -f printf format, -x/-X expand tabs, -m pattern filter.
  // pushln is print -nz with each argument on its own line.
  todo("bi_print");
}

int bi_printf(Shell& shell, const vec<str>& args) {
  // TODO(impl): printf [-v name] format args: format_printf(); status 1 if any argument was bad.
  todo("bi_printf");
}

}  // namespace myzsh
