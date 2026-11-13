#include <shell/cmdline.h>

#include <util/todo.h>

namespace myzsh {

bool Invocation::interactive_requested() const {
  // TODO(impl): an explicit -i/+i wins over the isatty() default, which is
  // decided by Shell::init.
  todo("Invocation::interactive_requested");
}

s::expected<Invocation, str> parse_command_line(const vec<str>& argv) {
  // TODO(impl): option letters map through lookup_option_letter with the
  // emulation derived from argv[0]. Hand-written rather than CLI11: zsh's
  // "+x" negation, clustered "-xv", "-o name" and "-c" taking the first
  // non-option are not expressible in a normal option parser.
  todo("parse_command_line");
}

}  // namespace myzsh
