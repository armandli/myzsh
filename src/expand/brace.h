#ifndef BRACE_H
#define BRACE_H

#include <util/aliases.h>
#include <util/qstring.h>

namespace myzsh {

// Brace expansion (xpandbraces() in Src/glob.c), applied after parameter
// expansion and before globbing, on unquoted braces only:
//   {a,b}c  {1..5}  {05..10..2}  {a..e}  {5..1}  nested {a,{b,c}}
//   brace_ccl (BRACE_CCL option): {abc} -> a b c, {a-c} -> a b c
// A brace group with no comma or range (and no BRACE_CCL) is left alone.
vec<QString> expand_braces(const QString& word, bool brace_ccl);

}  // namespace myzsh

#endif  // BRACE_H
