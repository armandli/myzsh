#include <gtest/gtest.h>

#include <zle/buffer.h>

namespace myzsh {
namespace {

LineBuffer make(strv text, size_t cursor) {
  LineBuffer buffer;
  buffer.text = str(text);
  buffer.cursor = cursor;
  return buffer;
}

TEST(LineBuffer, InsertAndDelete) {
  LineBuffer buffer;
  buffer.insert("hello");
  EXPECT_EQ(buffer.text, "hello");
  EXPECT_EQ(buffer.cursor, 5u);
  buffer.backward_char(2);
  buffer.insert("X");
  EXPECT_EQ(buffer.text, "helXlo");
  EXPECT_EQ(buffer.cursor, 4u);
  buffer.delete_backward();
  EXPECT_EQ(buffer.text, "hello");
  buffer.delete_forward(2);
  EXPECT_EQ(buffer.text, "hel");
  buffer.delete_forward();
  EXPECT_EQ(buffer.text, "hel");
}

TEST(LineBuffer, MovementClamps) {
  LineBuffer buffer = make("abc", 1);
  buffer.backward_char(5);
  EXPECT_EQ(buffer.cursor, 0u);
  buffer.forward_char(10);
  EXPECT_EQ(buffer.cursor, 3u);
  buffer.move_to(99);
  EXPECT_EQ(buffer.cursor, 3u);
  buffer.beginning_of_line();
  EXPECT_EQ(buffer.cursor, 0u);
  buffer.end_of_line();
  EXPECT_EQ(buffer.cursor, 3u);
}

TEST(LineBuffer, LineMotionInMultiLineBuffer) {
  LineBuffer buffer = make("one\ntwo", 5);
  buffer.beginning_of_line();
  EXPECT_EQ(buffer.cursor, 4u);
  buffer.move_to(1);
  buffer.end_of_line();
  EXPECT_EQ(buffer.cursor, 3u);
}

TEST(LineBuffer, MultibyteCharacters) {
  LineBuffer buffer;
  buffer.insert("a\xc3\xa9");
  buffer.backward_char();
  EXPECT_EQ(buffer.cursor, 1u);
  buffer.delete_forward();
  EXPECT_EQ(buffer.text, "a");
}

TEST(LineBuffer, EmacsWordMotion) {
  LineBuffer buffer = make("echo foo-bar baz", 0);
  buffer.forward_word();
  EXPECT_EQ(buffer.cursor, 4u);
  // '-' is in WORDCHARS, so foo-bar is one word.
  buffer.forward_word();
  EXPECT_EQ(buffer.cursor, 12u);
  buffer.backward_word();
  EXPECT_EQ(buffer.cursor, 5u);
  buffer.word_chars = "";
  buffer.move_to(5);
  buffer.forward_word();
  EXPECT_EQ(buffer.cursor, 8u);
}

TEST(LineBuffer, ViWordMotion) {
  LineBuffer buffer = make("ab.cd ef", 0);
  buffer.vi_forward_word(false);
  EXPECT_EQ(buffer.cursor, 2u);
  buffer.move_to(0);
  buffer.vi_forward_word(true);
  EXPECT_EQ(buffer.cursor, 6u);
  buffer.move_to(0);
  buffer.vi_forward_word_end(false);
  EXPECT_EQ(buffer.cursor, 1u);
  buffer.move_to(7);
  buffer.vi_backward_word(true);
  EXPECT_EQ(buffer.cursor, 6u);
}

TEST(LineBuffer, KillAndYank) {
  LineBuffer buffer = make("echo hello world", 5);
  buffer.kill_line();
  EXPECT_EQ(buffer.text, "echo ");
  ASSERT_FALSE(buffer.kill_ring.empty());
  EXPECT_EQ(buffer.kill_ring.front(), "hello world");
  buffer.yank();
  EXPECT_EQ(buffer.text, "echo hello world");
  EXPECT_EQ(buffer.cursor, 16u);
}

TEST(LineBuffer, ConsecutiveKillsAppend) {
  LineBuffer buffer = make("one two three", 13);
  buffer.backward_kill_word();
  buffer.backward_kill_word();
  EXPECT_EQ(buffer.text, "one ");
  EXPECT_EQ(buffer.kill_ring.front(), "two three");
}

TEST(LineBuffer, YankPopCyclesRing) {
  LineBuffer buffer = make("a b", 3);
  buffer.backward_kill_word();  // kills "b"
  buffer.move_to(0);
  buffer.kill_word();           // kills "a" (new entry: cursor moved)
  EXPECT_EQ(buffer.text, " ");
  buffer.yank();
  EXPECT_EQ(buffer.text, "a ");
  buffer.yank_pop();
  EXPECT_EQ(buffer.text, "b ");
}

TEST(LineBuffer, KillWholeLineAndBackward) {
  LineBuffer buffer = make("abc def", 3);
  buffer.backward_kill_line();
  EXPECT_EQ(buffer.text, " def");
  EXPECT_EQ(buffer.cursor, 0u);
  buffer.kill_whole_line();
  EXPECT_EQ(buffer.text, "");
}

TEST(LineBuffer, Region) {
  LineBuffer buffer = make("hello world", 0);
  buffer.set_mark();
  buffer.move_to(5);
  buffer.copy_region_as_kill();
  EXPECT_EQ(buffer.kill_ring.front(), "hello");
  EXPECT_EQ(buffer.text, "hello world");
  buffer.kill_region();
  EXPECT_EQ(buffer.text, " world");
  buffer.exchange_point_and_mark();
  EXPECT_EQ(buffer.cursor, 0u);
}

TEST(LineBuffer, Transpose) {
  LineBuffer chars = make("abc", 1);
  chars.transpose_chars();
  EXPECT_EQ(chars.text, "bac");
  EXPECT_EQ(chars.cursor, 2u);
  LineBuffer at_end = make("abc", 3);
  at_end.transpose_chars();
  EXPECT_EQ(at_end.text, "acb");
  LineBuffer words = make("one two", 5);
  words.transpose_words();
  EXPECT_EQ(words.text, "two one");
}

TEST(LineBuffer, CaseWords) {
  LineBuffer buffer = make("hello world", 0);
  buffer.upcase_word();
  EXPECT_EQ(buffer.text, "HELLO world");
  buffer.capitalize_word();
  EXPECT_EQ(buffer.text, "HELLO World");
  buffer.move_to(0);
  buffer.downcase_word();
  EXPECT_EQ(buffer.text, "hello World");
}

TEST(LineBuffer, UndoRedo) {
  LineBuffer buffer;
  buffer.begin_change();
  buffer.insert("abc");
  buffer.begin_change();
  buffer.insert("def");
  EXPECT_TRUE(buffer.undo());
  EXPECT_EQ(buffer.text, "abc");
  EXPECT_TRUE(buffer.undo());
  EXPECT_EQ(buffer.text, "");
  EXPECT_FALSE(buffer.undo());
  EXPECT_TRUE(buffer.redo());
  EXPECT_EQ(buffer.text, "abc");
}

TEST(LineBuffer, KillRingIsBounded) {
  LineBuffer buffer;
  buffer.kill_ring_max = 2;
  for (strv word : {"a ", "b ", "c "}) {
    buffer.insert(word);
    buffer.move_to(0);
    buffer.kill_line();
    buffer.insert("x");  // breaks kill appending
    buffer.kill_whole_line();
  }
  EXPECT_LE(buffer.kill_ring.size(), 2u);
}

}  // namespace
}  // namespace myzsh
