#ifndef TODO_H
#define TODO_H

#include <stdexcept>

#include <util/aliases.h>

namespace myzsh {

// Thrown by every unimplemented stub so a test fails with the name of the
// missing piece instead of crashing or silently passing.
struct NotImplemented : s::logic_error {
  using s::logic_error::logic_error;
};

[[noreturn]] inline void todo(strv what) {
  throw NotImplemented(str("not implemented: ") + str(what));
}

}  // namespace myzsh

#endif  // TODO_H
