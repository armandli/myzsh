#include <parse/text.h>

#include <util/todo.h>

namespace myzsh {

str list_to_text(const ast::List& list, int indent) {
  // TODO(impl): getpermtext()/gettext2() in Src/text.c. Separators: a list
  // item ending in & prints " &", &| prints " &|". Case items print as
  // "(pat|pat) body ;;". Words are printed exactly as written.
  todo("list_to_text");
}

str pipeline_to_text(const ast::Pipeline& pipeline) {
  // TODO(impl)
  todo("pipeline_to_text");
}

str function_to_text(strv name, const ast::List& body) {
  // TODO(impl)
  todo("function_to_text");
}

}  // namespace myzsh
