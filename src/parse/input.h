#ifndef INPUT_H
#define INPUT_H

#include <util/aliases.h>

namespace myzsh {

// Where the lexer gets its characters (Src/input.c). zsh parses a script
// file, a sourced file and stdin one top-level list at a time and runs each
// list before reading the next, so input must be pulled on demand rather
// than read whole:
//  - a parse error late in a script still runs the earlier lines;
//  - an alias defined in a script applies to its later lines;
//  - with `zsh < script` or `cmd | zsh`, `read` in the script consumes the
//    script's next lines, so the shell must not read stdin ahead of the
//    parser (see sync_position()).
// -c and eval use a StringInput and are still parsed whole.
struct InputSource {
  // Returned by next_char() when the input is exhausted.
  static constexpr int END_OF_INPUT = -1;

  virtual ~InputSource() = default;

  // Next byte (0-255), taking pushed-back text first, or END_OF_INPUT.
  int next_char();
  // Puts one character back; the lexer's lookahead.
  void unget(char c);
  // Pushes text to be read before anything else: alias replacement text
  // (zsh's inpush()). Pushed text is read in last-in, first-out order.
  void push_text(strv text);
  // Gives back bytes read from the underlying file but not yet consumed by
  // the lexer, so a command run next (a child, `read`, `exec`) starts at
  // the right position. Called by Shell::run_input before executing each
  // list. Pushed-back text is not affected.
  virtual void sync_position() {}

protected:
  // Next byte from the underlying source, ignoring pushed-back text.
  virtual int read_char() = 0;

private:
  // Stack of pushed-back text; the back of the string is read first.
  str mPushed;
};

// A string held in memory: -c, eval, $(...) bodies, tests.
struct StringInput : InputSource {
  explicit StringInput(strv source) : mSource(source) {}

protected:
  int read_char() override;

private:
  str mSource;
  size_t mPos = 0;
};

// A file descriptor: script files, sourced files, -s and stdin. The fd is
// not owned. On a seekable fd, reads go through a block buffer and
// sync_position() lseek()s back over the unread part; on a pipe or tty,
// bytes are read one at a time so nothing past the current list is
// consumed (zsh's SHIN handling, shinbufreset()).
struct FdInput : InputSource {
  explicit FdInput(int fd);

  void sync_position() override;
  bool seekable() const { return mSeekable; }

protected:
  int read_char() override;

private:
  int mFd;
  bool mSeekable = false;
  str mBuffer;
  size_t mPos = 0;
};

}  // namespace myzsh

#endif  // INPUT_H
