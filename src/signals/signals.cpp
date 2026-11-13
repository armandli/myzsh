#include <signals/signals.h>

#include <util/todo.h>

namespace myzsh {

opt<int> signal_number(strv name) {
  // TODO(impl): case-insensitive; "ERR" is an alias of ZERR.
  todo("signal_number");
}

str signal_name(int number) {
  // TODO(impl)
  todo("signal_name");
}

vec<str> signal_names() {
  // TODO(impl): index 0 is "EXIT", then real signals, then ZERR, DEBUG.
  todo("signal_names");
}

const Trap& TrapTable::get(int signal) const {
  // TODO(impl): a static default Trap for absent entries.
  todo("TrapTable::get");
}

void TrapTable::set(int signal, Trap trap) {
  // TODO(impl)
  todo("TrapTable::set");
}

void TrapTable::reset(int signal) {
  // TODO(impl)
  todo("TrapTable::reset");
}

vec<int> TrapTable::active() const {
  // TODO(impl)
  todo("TrapTable::active");
}

vec<str> TrapTable::listing() const {
  // TODO(impl)
  todo("TrapTable::listing");
}

void TrapTable::reset_for_subshell() {
  // TODO(impl)
  todo("TrapTable::reset_for_subshell");
}

void SignalManager::install(bool interactive, bool job_control) {
  // TODO(impl): sigaction with SA_RESTART off for SIGINT so reads abort.
  todo("SignalManager::install");
}

void SignalManager::reset_in_child() {
  // TODO(impl)
  todo("SignalManager::reset_in_child");
}

void SignalManager::catch_signal(int signal) {
  // TODO(impl)
  todo("SignalManager::catch_signal");
}

void SignalManager::default_signal(int signal) {
  // TODO(impl)
  todo("SignalManager::default_signal");
}

void SignalManager::ignore_signal(int signal) {
  // TODO(impl)
  todo("SignalManager::ignore_signal");
}

vec<int> SignalManager::take_pending() {
  // TODO(impl): a volatile sig_atomic_t flag per signal, read with the
  // signals blocked.
  todo("SignalManager::take_pending");
}

bool SignalManager::interrupted() {
  // TODO(impl)
  todo("SignalManager::interrupted");
}

void SignalManager::clear_interrupt() {
  // TODO(impl)
  todo("SignalManager::clear_interrupt");
}

void SignalManager::block_children() {
  // TODO(impl)
  todo("SignalManager::block_children");
}

void SignalManager::unblock_children() {
  // TODO(impl)
  todo("SignalManager::unblock_children");
}

}  // namespace myzsh
