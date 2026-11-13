#include <builtins/builtin.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

int bi_hash(Shell& shell, const vec<str>& args) {
  // TODO(impl): hash [-Ldfmrv] [name[=value]...]: list or add command hash entries; -d named
  // directories; -r empty the table; -f fill it; -m patterns.
  todo("bi_hash");
}

int bi_unhash(Shell& shell, const vec<str>& args) {
  // TODO(impl): unhash [-adfms] names / unfunction names (= unhash -f).
  todo("bi_unhash");
}

int bi_rehash(Shell& shell, const vec<str>& args) {
  // TODO(impl): rehash: empty the command hash table (and refill with HASH_DIRS).
  todo("bi_rehash");
}

}  // namespace myzsh
