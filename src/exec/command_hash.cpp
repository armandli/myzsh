#include <exec/command_hash.h>

#include <util/todo.h>

namespace myzsh {

opt<str> search_path(strv name, const vec<str>& path) {
  // TODO(impl): names containing '/' are not searched; return nullopt.
  todo("search_path");
}

opt<str> CommandHash::find(strv name, const vec<str>& path) {
  // TODO(impl): a stale entry (file gone) is dropped and searched again.
  todo("CommandHash::find");
}

void CommandHash::add(strv name, strv full_path) {
  // TODO(impl)
  todo("CommandHash::add");
}

bool CommandHash::remove(strv name) {
  // TODO(impl)
  todo("CommandHash::remove");
}

void CommandHash::clear() {
  // TODO(impl)
  todo("CommandHash::clear");
}

void CommandHash::fill(const vec<str>& path) {
  // TODO(impl): earlier directories win.
  todo("CommandHash::fill");
}

vec<s::pair<str, str>> CommandHash::entries() const {
  // TODO(impl)
  todo("CommandHash::entries");
}

}  // namespace myzsh
