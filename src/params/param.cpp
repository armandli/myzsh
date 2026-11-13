#include <params/param.h>

#include <util/error.h>
#include <util/todo.h>

namespace myzsh {

opt<s::reference_wrapper<const Param>> ParamTable::find(strv name) const {
  // TODO(impl): newest definition in mParams; specials are synthesised.
  todo("ParamTable::find");
}

bool ParamTable::is_set(strv name) const {
  // TODO(impl): declared-but-unset locals (PF_UNSET) are not set.
  todo("ParamTable::is_set");
}

opt<str> ParamTable::get_scalar(strv name) const {
  // TODO(impl)
  todo("ParamTable::get_scalar");
}

opt<vec<str>> ParamTable::get_array(strv name) const {
  // TODO(impl)
  todo("ParamTable::get_array");
}

opt<Assoc> ParamTable::get_assoc(strv name) const {
  // TODO(impl)
  todo("ParamTable::get_assoc");
}

opt<s::int64_t> ParamTable::get_integer(strv name) const {
  // TODO(impl): integer value or a scalar evaluated arithmetically.
  todo("ParamTable::get_integer");
}

void ParamTable::set_scalar(strv name, strv value) {
  // TODO(impl): assigning a scalar to an existing array replaces it with a
  // scalar (zsh behaviour), unless the name is special.
  todo("ParamTable::set_scalar");
}

void ParamTable::set_integer(strv name, s::int64_t value) {
  // TODO(impl)
  todo("ParamTable::set_integer");
}

void ParamTable::set_float(strv name, double value) {
  // TODO(impl)
  todo("ParamTable::set_float");
}

void ParamTable::set_array(strv name, vec<str> value) {
  // TODO(impl): an assoc assigned an array takes key/value pairs and throws
  // "bad set of key/value pairs for associative array" on odd counts.
  todo("ParamTable::set_array");
}

void ParamTable::set_assoc(strv name, Assoc value) {
  // TODO(impl)
  todo("ParamTable::set_assoc");
}

void ParamTable::append_scalar(strv name, strv value) {
  // TODO(impl)
  todo("ParamTable::append_scalar");
}

void ParamTable::append_array(strv name, const vec<str>& value) {
  // TODO(impl)
  todo("ParamTable::append_array");
}

bool ParamTable::unset(strv name) {
  // TODO(impl): unsetting a local leaves a PF_UNSET placeholder in its scope
  // so the outer value does not reappear until the function returns.
  todo("ParamTable::unset");
}

Param& ParamTable::declare(strv name, ParamType type, s::uint32_t flags,
                           bool local_scope)
{
  // TODO(impl): local_scope && mLevel > 0 && existing definition is from an
  // outer level => push a new Param; otherwise convert in place.
  todo("ParamTable::declare");
}

void ParamTable::set_flags(strv name, s::uint32_t add, s::uint32_t remove) {
  // TODO(impl)
  todo("ParamTable::set_flags");
}

void ParamTable::push_scope() {
  // TODO(impl)
  todo("ParamTable::push_scope");
}

void ParamTable::pop_scope() {
  // TODO(impl): drop every definition with level == mLevel.
  todo("ParamTable::pop_scope");
}

void ParamTable::add_special(strv name, SpecialParam special,
                             s::uint32_t flags)
{
  // TODO(impl)
  todo("ParamTable::add_special");
}

vec<str> ParamTable::names() const {
  // TODO(impl)
  todo("ParamTable::names");
}

vec<str> ParamTable::environment() const {
  // TODO(impl)
  todo("ParamTable::environment");
}

void ParamTable::import_environment(char** envp) {
  // TODO(impl): every valid identifier becomes an exported scalar.
  todo("ParamTable::import_environment");
}

Param& ParamTable::lookup_or_create(strv name) {
  // TODO(impl)
  todo("ParamTable::lookup_or_create");
}

str format_float(double value) {
  // TODO(impl): convfloat() in Src/params.c with %.17g, appending '.' when
  // the result looks like an integer.
  todo("format_float");
}

str format_float(double value, bool exponent, int precision) {
  // TODO(impl)
  todo("format_float");
}

str format_integer(s::int64_t value, int base, bool with_prefix) {
  // TODO(impl): digits above 9 are upper-case; negative values "-16#FF".
  todo("format_integer");
}

}  // namespace myzsh
