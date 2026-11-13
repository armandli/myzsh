#include <zle/buffer.h>

#include <util/todo.h>

namespace myzsh {

void LineBuffer::insert(strv text_to_insert) {
  // TODO(impl)
  todo("LineBuffer::insert");
}

void LineBuffer::delete_forward(size_t count) {
  // TODO(impl)
  todo("LineBuffer::delete_forward");
}

void LineBuffer::delete_backward(size_t count) {
  // TODO(impl)
  todo("LineBuffer::delete_backward");
}

void LineBuffer::move_to(size_t position) {
  // TODO(impl): clamp to [0, size].
  todo("LineBuffer::move_to");
}

void LineBuffer::beginning_of_line() {
  // TODO(impl): of the current line in a multi-line buffer.
  todo("LineBuffer::beginning_of_line");
}

void LineBuffer::end_of_line() {
  // TODO(impl)
  todo("LineBuffer::end_of_line");
}

void LineBuffer::forward_char(size_t count) {
  // TODO(impl)
  todo("LineBuffer::forward_char");
}

void LineBuffer::backward_char(size_t count) {
  // TODO(impl)
  todo("LineBuffer::backward_char");
}

void LineBuffer::forward_word(size_t count) {
  // TODO(impl): skip non-word chars, then word chars (zle_word.c).
  todo("LineBuffer::forward_word");
}

void LineBuffer::backward_word(size_t count) {
  // TODO(impl)
  todo("LineBuffer::backward_word");
}

void LineBuffer::vi_forward_word(bool blank_words) {
  // TODO(impl)
  todo("LineBuffer::vi_forward_word");
}

void LineBuffer::vi_backward_word(bool blank_words) {
  // TODO(impl)
  todo("LineBuffer::vi_backward_word");
}

void LineBuffer::vi_forward_word_end(bool blank_words) {
  // TODO(impl)
  todo("LineBuffer::vi_forward_word_end");
}

void LineBuffer::kill_line() {
  // TODO(impl)
  todo("LineBuffer::kill_line");
}

void LineBuffer::backward_kill_line() {
  // TODO(impl)
  todo("LineBuffer::backward_kill_line");
}

void LineBuffer::kill_whole_line() {
  // TODO(impl)
  todo("LineBuffer::kill_whole_line");
}

void LineBuffer::kill_word() {
  // TODO(impl)
  todo("LineBuffer::kill_word");
}

void LineBuffer::backward_kill_word() {
  // TODO(impl)
  todo("LineBuffer::backward_kill_word");
}

void LineBuffer::kill_region() {
  // TODO(impl)
  todo("LineBuffer::kill_region");
}

void LineBuffer::copy_region_as_kill() {
  // TODO(impl)
  todo("LineBuffer::copy_region_as_kill");
}

void LineBuffer::yank() {
  // TODO(impl)
  todo("LineBuffer::yank");
}

void LineBuffer::yank_pop() {
  // TODO(impl)
  todo("LineBuffer::yank_pop");
}

void LineBuffer::transpose_chars() {
  // TODO(impl): at end of line swaps the two chars before the cursor.
  todo("LineBuffer::transpose_chars");
}

void LineBuffer::transpose_words() {
  // TODO(impl)
  todo("LineBuffer::transpose_words");
}

void LineBuffer::upcase_word() {
  // TODO(impl)
  todo("LineBuffer::upcase_word");
}

void LineBuffer::downcase_word() {
  // TODO(impl)
  todo("LineBuffer::downcase_word");
}

void LineBuffer::capitalize_word() {
  // TODO(impl)
  todo("LineBuffer::capitalize_word");
}

void LineBuffer::begin_change() {
  // TODO(impl): snapshot unless identical to the last one; clear redo.
  todo("LineBuffer::begin_change");
}

bool LineBuffer::undo() {
  // TODO(impl)
  todo("LineBuffer::undo");
}

bool LineBuffer::redo() {
  // TODO(impl)
  todo("LineBuffer::redo");
}

void LineBuffer::push_kill(strv killed, bool prepend) {
  // TODO(impl)
  todo("LineBuffer::push_kill");
}

}  // namespace myzsh
