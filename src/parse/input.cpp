#include <parse/input.h>

#include <util/todo.h>

namespace myzsh {

int InputSource::next_char() {
  // TODO(impl): pop the back of mPushed if any, else read_char().
  todo("InputSource::next_char");
}

void InputSource::unget(char c) {
  // TODO(impl): append c to mPushed.
  todo("InputSource::unget");
}

void InputSource::push_text(strv text) {
  // TODO(impl): append text reversed to mPushed so it reads front to back.
  todo("InputSource::push_text");
}

int StringInput::read_char() {
  // TODO(impl)
  todo("StringInput::read_char");
}

FdInput::FdInput(int fd) : mFd(fd) {
  // TODO(impl): mSeekable = lseek(fd, 0, SEEK_CUR) succeeds and the fd is
  // a regular file (fstat S_ISREG). Keep this free of throwing todo() so
  // run_file can be reached by tests of other parts.
}

int FdInput::read_char() {
  // TODO(impl): seekable: refill mBuffer with read() in blocks (zsh uses
  // SHINBUFSIZE 8192). Otherwise read() one byte. Retry on EINTR; any other
  // error or 0 is END_OF_INPUT.
  todo("FdInput::read_char");
}

void FdInput::sync_position() {
  // TODO(impl): seekable only: lseek(mFd, -(mBuffer.size() - mPos),
  // SEEK_CUR) and drop the buffer. Nothing to do for unbuffered fds.
  todo("FdInput::sync_position");
}

}  // namespace myzsh
