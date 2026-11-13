#ifndef PRINTF_FORMAT_H
#define PRINTF_FORMAT_H

#include <expected>

#include <util/aliases.h>

namespace myzsh {

struct Shell;

// printf / print -f formatting (bin_print in Src/builtin.c):
//   %s %b (escapes) %q (shell-quoted) %c %d %i %o %u %x %X %e %E %f %F %g
//   %G %a %A %% with flags - + space # 0 ' width/precision (and *),
//   %N$ argument indexes. Numeric arguments are arithmetic expressions in
//   zsh; 'c / "c give a character code. The format is reused while
//   arguments remain; missing arguments are empty / zero.
struct PrintfResult {
  str output;
  // Bad numeric argument seen: printf returns 1 but still prints.
  bool had_error = false;
  vec<str> errors;
};

// The shell evaluates numeric arguments as arithmetic.
s::expected<PrintfResult, str> format_printf(strv format,
                                             const vec<str>& args,
                                             Shell& shell);

}  // namespace myzsh

#endif  // PRINTF_FORMAT_H
