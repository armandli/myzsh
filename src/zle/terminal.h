#ifndef TERMINAL_H
#define TERMINAL_H

#include <termios.h>

#include <util/aliases.h>

namespace myzsh {

// Result of reading one key byte. EOF, a signal and a timeout must be told
// apart: treating EOF like "no key yet" makes the editor spin forever on a
// closed terminal, and a signal (SIGCHLD with NOTIFY, SIGWINCH, a trap)
// must be handled while the editor waits.
struct KeyRead {
  enum class Status : int {
    Byte,         // `byte` holds the key
    EndOfFile,    // terminal closed or hung up
    Interrupted,  // a signal arrived: run Shell::dispatch_signals, retry
    TimedOut,     // timeout_ms elapsed (multi-byte sequences, sched)
  };

  Status status = Status::EndOfFile;
  char byte = 0;

  static KeyRead of(char c) { return {Status::Byte, c}; }
};

// Raw-mode terminal access for ZLE. RAII: the constructor saves the current
// settings, enter_raw() switches to character-at-a-time without echo, and
// the destructor restores (also needed before running commands).
struct Terminal {
  explicit Terminal(int fd);
  ~Terminal();
  Terminal(const Terminal&) = delete;
  Terminal& operator=(const Terminal&) = delete;

  bool is_tty() const;
  // Also turns on bracketed paste ("\e[?2004h") unless
  // zle_bracketed_paste is empty; restore() turns it off.
  void enter_raw();
  void restore();

  // One byte; timeout_ms < 0 waits forever.
  KeyRead read_byte(int timeout_ms);
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
