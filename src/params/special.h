#ifndef SPECIAL_H
#define SPECIAL_H

#include <util/aliases.h>

namespace myzsh {

struct Shell;

// Registers the special parameters of Src/params.c special_params[]:
//   $? status pipestatus  $$ $! $# $* $@ argv $0 $- $_ ARGC
//   RANDOM SECONDS LINENO PPID SHLVL ERRNO TTY TTYIDLE
//   PWD OLDPWD HOME IFS PATH/path FPATH/fpath CDPATH/cdpath MANPATH/manpath
//   MODULE_PATH/module_path PSVAR/psvar HISTSIZE SAVEHIST HISTFILE
//   HISTCMD UID EUID GID EGID USERNAME HOST OSTYPE MACHTYPE VENDOR
//   ZSH_VERSION ZSH_PATCHLEVEL ZSH_NAME ZSH_SUBSHELL ZSH_ARGZERO
//   COLUMNS LINES KEYTIMEOUT WORDCHARS PROMPT PS1 PS2 PS3 PS4 RPS1 ...
//   funcstack funcfiletrace (zsh/parameter) options aliases functions
//   commands parameters jobtexts jobstates dirstack history (zsh/parameter)
void install_special_params(Shell& shell);

}  // namespace myzsh

#endif  // SPECIAL_H
