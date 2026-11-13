#include <builtins/builtin.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

int bi_read(Shell& shell, const vec<str>& args) {
  // TODO(impl): read [-rszpqeAclneE] [-k[n]] [-u fd] [-t timeout] [-d delim] [name?prompt] [names...]
  // Splits on IFS into names (last gets the rest), default name REPLY, -A array, -r no
  // backslash processing, -s no echo, -t timeout (status 1), -k N chars, -q y/n,
  // -d delimiter, -e/-E echo, -p from coprocess, -z/-l/-c from editor buffer.
  // Status 1 at EOF (values still assigned).
  todo("bi_read");
}

int bi_getln(Shell& shell, const vec<str>& args) {
  // TODO(impl): getln [-AclneE] name: pop a line from the editor buffer stack (print -z).
  todo("bi_getln");
}

int bi_vared(Shell& shell, const vec<str>& args) {
  // TODO(impl): vared [-ache] [-p prompt] [-r rprompt] name: edit a parameter's value with ZLE.
  todo("bi_vared");
}

}  // namespace myzsh
