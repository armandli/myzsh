#include <exec/functions.h>

#include <parse/parser.h>
#include <util/todo.h>

namespace myzsh {

opt<s::reference_wrapper<const ShellFunction>> FunctionTable::find(
    strv name) const
{
  // TODO(impl)
  todo("FunctionTable::find");
}

void FunctionTable::define(ShellFunction function) {
  // TODO(impl): redefinition replaces; a function named TRAPxxx must also
  // be registered as a trap by the caller.
  todo("FunctionTable::define");
}

bool FunctionTable::remove(strv name) {
  // TODO(impl)
  todo("FunctionTable::remove");
}

vec<str> FunctionTable::names() const {
  // TODO(impl): sorted.
  todo("FunctionTable::names");
}

void FunctionTable::mark_autoload(strv name, s::uint32_t flags) {
  // TODO(impl): no-op for an already defined function.
  todo("FunctionTable::mark_autoload");
}

void FunctionTable::set_flags(strv name, s::uint32_t add,
                              s::uint32_t remove)
{
  // TODO(impl)
  todo("FunctionTable::set_flags");
}

s::expected<AutoloadResult, str> load_autoload(strv name,
                                               const vec<str>& fpath)
{
  // TODO(impl): error text "function definition file not found".
  todo("load_autoload");
}

}  // namespace myzsh
