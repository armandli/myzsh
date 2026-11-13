#include <zle/terminal.h>

#include <util/todo.h>

namespace myzsh {

Terminal::Terminal(int fd) : mFd(fd) {
  // TODO(impl): tcgetattr into mSaved when fd is a tty. Keep this free of
  // throwing todo() so a Shell can be built without a terminal.
}

Terminal::~Terminal() {
  // TODO(impl): restore() if mRaw.
}

bool Terminal::is_tty() const {
  // TODO(impl)
  todo("Terminal::is_tty");
}

void Terminal::enter_raw() {
  // TODO(impl): clear ICANON ECHO IEXTEN, ISIG kept so ^C still signals;
  // VMIN 1 VTIME 0.
  todo("Terminal::enter_raw");
}

void Terminal::restore() {
  // TODO(impl)
  todo("Terminal::restore");
}

opt<char> Terminal::read_byte(int timeout_ms) {
  // TODO(impl): poll() then read().
  todo("Terminal::read_byte");
}

void Terminal::write(strv text) {
  // TODO(impl)
  todo("Terminal::write");
}

Terminal::Size Terminal::size() const {
  // TODO(impl): TIOCGWINSZ, falling back to $COLUMNS/$LINES.
  todo("Terminal::size");
}

}  // namespace myzsh
