#include <params/special.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

void install_special_params(Shell& shell) {
  // TODO(impl): add a SpecialParam for each entry listed in special.h. Tied
  // pairs (PATH/path etc.) are regular params with PF_TIED. SECONDS counts
  // from shell start and can be assigned to reset the origin. RANDOM is
  // 0..32767 and assigning seeds it. Assigning PATH clears the command hash.
  todo("install_special_params");
}

}  // namespace myzsh
