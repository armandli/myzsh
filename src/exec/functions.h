#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <cstdint>
#include <expected>

#include <parse/ast.h>
#include <util/aliases.h>

namespace myzsh {

enum FunctionFlag : s::uint32_t {
  FF_NONE = 0,
  FF_AUTOLOAD = 1u << 0,      // body not loaded yet
  FF_TRACE = 1u << 1,         // functions -t: xtrace while running
  FF_TRACE_ONCE = 1u << 2,    // functions -T
  FF_NO_ALIAS = 1u << 3,      // autoload -U
  FF_KSH_AUTOLOAD = 1u << 4,  // autoload -k
  FF_ZSH_AUTOLOAD = 1u << 5,  // autoload -z
  FF_WARN_NESTED = 1u << 6,   // functions -W
};

struct ShellFunction {
  str name;
  sptr<const ast::List> body;
  s::uint32_t flags = FF_NONE;
  // Where it was defined or autoloaded from (for $funcsourcetrace).
  str filename;
  int line = 0;
};

// Src/hashtable.c shfunctab.
struct FunctionTable {
  opt<s::reference_wrapper<const ShellFunction>> find(strv name) const;
  void define(ShellFunction function);
  bool remove(strv name);
  vec<str> names() const;
  // autoload name: an undefined stub to be resolved on first call.
  void mark_autoload(strv name, s::uint32_t flags);
  void set_flags(strv name, s::uint32_t add, s::uint32_t remove);

private:
  umap<str, ShellFunction> mFunctions;
};

// Finds `name` in fpath and parses it. zsh-style autoload: the file body is
// the function body; if the file only defines a function of the same name
// (ksh style, or KSH_AUTOLOAD), the file is executed and that definition is
// used. Returns nullopt with an error message if not found.
struct AutoloadResult {
  sptr<const ast::List> body;
  str filename;
  // Run the file once and then call the function it defined.
  bool ksh_style = false;
};
s::expected<AutoloadResult, str> load_autoload(strv name,
                                               const vec<str>& fpath);

}  // namespace myzsh

#endif  // FUNCTIONS_H
