#include <cstdio>
#include <exception>

#include <shell/cmdline.h>
#include <shell/shell.h>
#include <util/aliases.h>
#include <util/todo.h>

int main(int argc, char** argv, char** envp) {
  namespace mz = myzsh;
  vec<str> arguments(argv, argv + argc);
  try {
    auto invocation = mz::parse_command_line(arguments);
    if (not invocation) {
      s::fprintf(stderr, "zsh: %s\n", invocation.error().c_str());
      return 1;
    }
    mz::Shell shell;
    shell.init(*invocation, envp);
    return shell.run(*invocation);
  } catch (const mz::NotImplemented& error) {
    // Lets the integration tests fail with a readable reason while the
    // shell is being written.
    s::fprintf(stderr, "myzsh: %s\n", error.what());
    return 70;
  }
}
