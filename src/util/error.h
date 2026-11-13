#ifndef ERROR_H
#define ERROR_H

#include <stdexcept>

#include <util/aliases.h>

namespace myzsh {

// Equivalent of zsh's errflag: aborts the current command line (or the whole
// script when non-interactive). The message excludes the "zsh:LINE: " prefix;
// Shell::report_error adds it.
struct ShellError : s::runtime_error {
  using s::runtime_error::runtime_error;
};

struct ParseError {
  str message;
  int line = 0;
  // true when the input ended in the middle of a construct, so an interactive
  // shell should print PS2 and read another line instead of reporting it.
  bool incomplete = false;
};

}  // namespace myzsh

#endif  // ERROR_H
