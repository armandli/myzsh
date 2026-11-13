#include <builtins/printf_format.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

s::expected<PrintfResult, str> format_printf(strv format,
                                             const vec<str>& args,
                                             Shell& shell)
{
  // TODO(impl): format errors ("%z: invalid directive") are the unexpected
  // case; %b stops all output at \c.
  todo("format_printf");
}

}  // namespace myzsh
