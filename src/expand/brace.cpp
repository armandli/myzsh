#include <expand/brace.h>

#include <util/todo.h>

namespace myzsh {

vec<QString> expand_braces(const QString& word, bool brace_ccl) {
  // TODO(impl): find the first unquoted '{' with a matching '}', expand it,
  // and recurse on every result. Numeric ranges keep the widest zero
  // padding of the endpoints; a negative step reverses order.
  todo("expand_braces");
}

}  // namespace myzsh
