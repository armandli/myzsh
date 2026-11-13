#ifndef TERMINAL_H
#define TERMINAL_H

#include <termios.h>

#include <util/aliases.h>

namespace myzsh {

// Raw-mode terminal access for ZLE. RAII: the constructor saves the current
// settings, enter_raw() switches to character-at-a-time without echo, and
// the destructor restores (also needed before running commands).
struct Terminal {
  explicit Terminal(int fd);
  ~Terminal();
  Terminal(const Terminal&) = delete;
  Terminal& operator=(const Terminal&) = delete;

  bool is_tty() const;
  void enter_raw();
  void restore();

  // One byte, or nullopt on EOF / timeout (timeout_ms < 0 waits forever).
  // Interrupted by a signal: returns nullopt and errno is EINTR.
  opt<char> read_byte(int timeout_ms);
  void write(strv text);

  struct Size {
    int columns = 80;
    int lines = 24;
  };
  Size size() const;

private:
  int mFd;
  termios mSaved{};
  bool mHaveSaved = false;
  bool mRaw = false;
};

}  // namespace myzsh

#endif  // TERMINAL_H
