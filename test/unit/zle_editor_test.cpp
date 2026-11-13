#include <gtest/gtest.h>

#include <algorithm>

#include <support/test_support.h>
#include <zle/editor.h>

namespace myzsh {
namespace {

struct EditorTest : ::testing::Test {
  uptr<Shell> shell = test::make_shell();
  LineEditor editor{*shell};

  void SetUp() override { editor.init(); }

  void type(strv text) {
    for (char c : text) {
      editor.process_keys(str(1, c));
    }
  }
};

TEST_F(EditorTest, SelfInsertAndAccept) {
  type("echo hi");
  EXPECT_EQ(editor.buffer().text, "echo hi");
  EXPECT_FALSE(editor.process_keys("\r"));
  EXPECT_TRUE(editor.finished());
}

TEST_F(EditorTest, EmacsBindings) {
  type("world");
  editor.process_keys("\x01");  // ^A
  type("hello ");
  EXPECT_EQ(editor.buffer().text, "hello world");
  editor.process_keys("\x05");  // ^E
  editor.process_keys("\x17");  // ^W
  EXPECT_EQ(editor.buffer().text, "hello ");
  editor.process_keys("\x19");  // ^Y
  EXPECT_EQ(editor.buffer().text, "hello world");
  editor.process_keys("\x1f");  // ^_ undo
  EXPECT_EQ(editor.buffer().text, "hello ");
}

TEST_F(EditorTest, EscapeSequenceArrows) {
  type("abc");
  editor.process_keys("\x1b[D");
  EXPECT_EQ(editor.buffer().cursor, 2u);
  editor.process_keys("\x1b[C");
  EXPECT_EQ(editor.buffer().cursor, 3u);
}

TEST_F(EditorTest, HistoryNavigation) {
  HistoryOptions options;
  shell->history.add("first", 0, options);
  shell->history.add("second", 0, options);
  type("draft");
  editor.process_keys("\x10");  // ^P
  EXPECT_EQ(editor.buffer().text, "second");
  editor.process_keys("\x10");
  EXPECT_EQ(editor.buffer().text, "first");
  editor.process_keys("\x0e");  // ^N
  editor.process_keys("\x0e");
  EXPECT_EQ(editor.buffer().text, "draft");
}

TEST_F(EditorTest, EofOnEmptyLine) {
  editor.process_keys("\x04");  // ^D
  EXPECT_TRUE(editor.eof());
}

TEST_F(EditorTest, ViModeSwitching) {
  editor.keymaps().link("viins", "main");
  editor.keymaps().select("main");
  type("abc");
  editor.process_keys("\x1b");
  EXPECT_EQ(editor.keymaps().selected(), "vicmd");
  editor.process_keys("0");
  EXPECT_EQ(editor.buffer().cursor, 0u);
  editor.process_keys("x");
  EXPECT_EQ(editor.buffer().text, "bc");
  editor.process_keys("A");
  type("d");
  EXPECT_EQ(editor.buffer().text, "bcd");
}

TEST_F(EditorTest, CallWidgetByName) {
  type("abc");
  EXPECT_TRUE(editor.call_widget("beginning-of-line"));
  EXPECT_EQ(editor.buffer().cursor, 0u);
  EXPECT_FALSE(editor.call_widget("no-such-widget"));
}

TEST_F(EditorTest, CustomWidget) {
  editor.define_widget("insert-x", [](LineEditor& e) {
    e.buffer().insert("x");
    return true;
  });
  editor.keymaps().get("main")->get().bind("\x18" "x", {"insert-x", false});
  editor.process_keys("\x18" "x");
  EXPECT_EQ(editor.buffer().text, "x");
  vec<str> names = editor.widget_names();
  EXPECT_NE(s::ranges::find(names, "insert-x"), names.end());
}

TEST_F(EditorTest, UserWidgetFunctionSeesBuffer) {
  shell->run_string("upper() { BUFFER=${(U)BUFFER} }");
  editor.define_user_widget("upper-widget", "upper");
  type("abc");
  editor.call_widget("upper-widget");
  EXPECT_EQ(editor.buffer().text, "ABC");
}

TEST_F(EditorTest, ReadLineFromKeySource) {
  str keys = "ls -l\r";
  size_t index = 0;
  auto line = editor.read_line("% ", "", [&](int) -> opt<char> {
    if (index < keys.size()) {
      return keys[index++];
    }
    return s::nullopt;
  });
  EXPECT_EQ(line, "ls -l");
}

}  // namespace
}  // namespace myzsh
