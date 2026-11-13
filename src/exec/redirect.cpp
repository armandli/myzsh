#include <exec/redirect.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

Redirector::~Redirector() {
  // Destructor must not throw; restore() is only meaningful once apply()
  // has saved something.
  if (not mSaved.empty()) {
    try {
      restore();
    } catch (...) {
    }
  }
}

void Redirector::apply(const vec<ast::Redirect>& redirects) {
  // TODO(impl): expand targets with Expander::expand_scalar (a target that
  // globs to several files is "ambiguous redirect"); MergeOut with a word
  // that is not a number (">& file") is ErrWrite.
  todo("Redirector::apply");
}

void Redirector::restore() {
  // TODO(impl): restore in reverse order; wait for multios tee processes.
  todo("Redirector::restore");
}

void Redirector::redirect_fd(int target_fd, int source_fd) {
  // TODO(impl): when mSave, dup target_fd to >= 10 with FD_CLOEXEC first.
  todo("Redirector::redirect_fd");
}

int Redirector::open_target(const ast::Redirect& redirect, strv path) {
  // TODO(impl): flags per RedirType; APPEND_CREATE, CLOBBER_EMPTY.
  todo("Redirector::open_target");
}

}  // namespace myzsh
