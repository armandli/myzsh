#ifndef CMDLINE_H
#define CMDLINE_H

#include <expected>

#include <shell/options.h>
#include <util/aliases.h>

namespace myzsh {

// How the shell was invoked (parseargs() in Src/init.c):
//   myzsh [-/+options] [-c command | script] [args...]
//   -c cmd        run cmd ($0 = first arg after it, if any)
//   -s            read commands from stdin, args are positionals
//   -i -l -f -x -e -v ... single-letter options (+X turns off)
//   -o name / +o name, --name / --no-name   long options
//   -b / - / --   end of options
//   --version --help --emulate MODE
// A login shell is also detected from argv[0] starting with '-'. argv[0]
// basename "sh"/"ksh"/"csh" selects that emulation.
struct Invocation {
  str argv0;
  opt<str> command;      // -c
  opt<str> script;       // first non-option argument without -c/-s
  vec<str> arguments;    // positional parameters
  // Explicit option settings in command-line order.
  vec<s::pair<Option, bool>> options;
  Emulation emulation = Emulation::Zsh;
  bool read_stdin = false; // -s, or no script and no -c
  bool login = false;
  bool show_version = false;
  bool show_help = false;

  bool interactive_requested() const;
};

// Error text matches zsh: "bad option: -q", "no such option: foo",
// "string expected after -o".
s::expected<Invocation, str> parse_command_line(const vec<str>& argv);

}  // namespace myzsh

#endif  // CMDLINE_H
