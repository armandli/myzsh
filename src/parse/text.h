#ifndef TEXT_H
#define TEXT_H

#include <parse/ast.h>
#include <util/aliases.h>

namespace myzsh {

// AST -> canonical source text (Src/text.c). Used by `functions`, `which`,
// $functions, job titles and xtrace of compound commands. The format matches
// zsh: one command per line, nested bodies indented with one tab, e.g.
//   if true
//   then
//   	echo yes
//   fi
str list_to_text(const ast::List& list, int indent = 0);
// Single-line form used for job titles ("a | b && c").
str pipeline_to_text(const ast::Pipeline& pipeline);
// "name () {\n\tbody\n}" as printed by `functions name`.
str function_to_text(strv name, const ast::List& body);

}  // namespace myzsh

#endif  // TEXT_H
