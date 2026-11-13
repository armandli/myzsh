#ifndef TILDE_H
#define TILDE_H

#include <util/aliases.h>
#include <util/qstring.h>

namespace myzsh {

struct Shell;

// Filename expansion (filesub() in Src/subst.c) on an unquoted leading
//   ~        $HOME          ~user    user's home
//   ~+       $PWD           ~-       $OLDPWD
//   ~N ~+N ~-N  directory stack entries
//   ~name    named directory (hash -d / AUTO_NAME_DIRS)
//   =cmd     path of cmd (EQUALS option)
// In assignments (and with MAGIC_EQUAL_SUBST in arguments) also after each
// unquoted ':' or the first '='. Throws ShellError("no such user or named
// directory: x") / ("x not found") on failure.
QString expand_tilde(const QString& word, Shell& shell, bool assignment);

}  // namespace myzsh

#endif  // TILDE_H
