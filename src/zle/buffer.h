#ifndef BUFFER_H
#define BUFFER_H

#include <util/aliases.h>

namespace myzsh {

// The editing buffer behind ZLE ($BUFFER, $CURSOR, $MARK, $CUTBUFFER,
// $killring), with the primitive operations the widgets are built from
// (Src/Zle/zle_utils.c, zle_misc.c, zle_word.c, zle_move.c).
// Cursor and mark are byte offsets into text and always on a character
// boundary.
struct LineBuffer {
  str text;
  size_t cursor = 0;
  size_t mark = 0;

  // Characters that count as part of a word besides alphanumerics
  // ($WORDCHARS, default "*?_-.[]~=/&;!#$%^(){}<>").
  str word_chars = "*?_-.[]~=/&;!#$%^(){}<>";

  void insert(strv text_to_insert);
  void delete_forward(size_t count = 1);
  void delete_backward(size_t count = 1);
  void move_to(size_t position);
  void beginning_of_line();
  void end_of_line();
  void forward_char(size_t count = 1);
  void backward_char(size_t count = 1);

  // Emacs word motion: forward-word stops after the end of a word,
  // backward-word at its start.
  void forward_word(size_t count = 1);
  void backward_word(size_t count = 1);
  // vi motions: w/b/e on "words" (alnum+_ vs punctuation runs) and
  // W/B/E on blank-separated words.
  void vi_forward_word(bool blank_words);
  void vi_backward_word(bool blank_words);
  void vi_forward_word_end(bool blank_words);

  // Killing pushes onto the kill ring; consecutive kills append.
  void kill_line();             // cursor to end
  void backward_kill_line();    // start to cursor
  void kill_whole_line();
  void kill_word();
  void backward_kill_word();
  void kill_region();
  void copy_region_as_kill();
  void yank();
  // After yank: replace the yanked text with the previous ring entry.
  void yank_pop();

  void transpose_chars();
  void transpose_words();
  void upcase_word();
  void downcase_word();
  void capitalize_word();

  void exchange_point_and_mark() { s::swap(cursor, mark); }
  void set_mark() { mark = cursor; }

  // Undo history: each widget invocation is one undo step.
  void begin_change();
  bool undo();
  bool redo();

  // Kill ring, newest first ($CUTBUFFER is kill_ring.front()).
  vec<str> kill_ring;
  size_t kill_ring_max = 8;

protected:
  void push_kill(strv killed, bool prepend);

private:
  struct Snapshot {
    str text;
    size_t cursor;
  };
  vec<Snapshot> mUndo;
  vec<Snapshot> mRedo;
  // Last command was a kill (append) / yank (yank-pop allowed).
  bool mLastWasKill = false;
  size_t mYankStart = 0;
  size_t mYankIndex = 0;
  bool mLastWasYank = false;
};

}  // namespace myzsh

#endif  // BUFFER_H
