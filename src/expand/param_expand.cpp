#include <expand/param_expand.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

s::expected<ParamExpr, ParseError> parse_param_expr(strv inner) {
  // TODO(impl): order in the source is
  //   [(flags)] [# | + | = | == | ~ | ~~ | ^ | ^^]* (name | ${...} | "...")
  //   [subscript]* [operator operand]
  // "${#}" is $#, "${#x}" length of x; "${-}" is $-. A ':' followed by a
  // modifier letter is Modifiers, by a digit/space/'(' is Substring, by one
  // of - = ? + # | * ^ the corresponding op.
  todo("parse_param_expr");
}

ExpansionValue ParamExpander::expand(const ParamExpr& expr,
                                     bool in_double_quotes)
{
  // TODO(impl): paramsubst() in Src/subst.c is the reference. Unset
  // parameter with NO_UNSET, and :? with no message (whether unset or
  // empty), throw ShellError("NAME: parameter not set"); :?msg throws
  // "NAME: msg".
  todo("ParamExpander::expand");
}

}  // namespace myzsh
