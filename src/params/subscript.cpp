#include <params/subscript.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

Subscript parse_subscript(strv text) {
  // TODO(impl): optional leading "(flags)", then "@", "*", "a,b" or "a".
  // A comma inside parentheses or quotes is not a range separator.
  todo("parse_subscript");
}

vec<str> SubscriptEvaluator::read_array(const vec<str>& array,
                                        const Subscript& subscript)
{
  // TODO(impl): (i) returns the index as a one-element result, or
  // size+1 when nothing matches; (I) returns 0 when nothing matches.
  todo("SubscriptEvaluator::read_array");
}

str SubscriptEvaluator::read_scalar(strv value, const Subscript& subscript) {
  // TODO(impl): characters by default; (w) switches to words.
  todo("SubscriptEvaluator::read_scalar");
}

vec<str> SubscriptEvaluator::read_assoc(const Assoc& assoc,
                                        const Subscript& subscript)
{
  // TODO(impl): [key] value lookup; (r)/(R) values matching the pattern;
  // (i)/(I) keys matching the pattern; (k)/(K) values whose keys, used as
  // patterns, match the subscript text.
  todo("SubscriptEvaluator::read_assoc");
}

void SubscriptEvaluator::assign_array(vec<str>& array,
                                      const Subscript& subscript,
                                      const vec<str>& value)
{
  // TODO(impl)
  todo("SubscriptEvaluator::assign_array");
}

void SubscriptEvaluator::assign_scalar(str& value, const Subscript& subscript,
                                       strv text)
{
  // TODO(impl)
  todo("SubscriptEvaluator::assign_scalar");
}

s::int64_t SubscriptEvaluator::eval_index(strv expression) {
  // TODO(impl): ArithEvaluator on the expression.
  todo("SubscriptEvaluator::eval_index");
}

}  // namespace myzsh
