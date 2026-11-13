#include <gtest/gtest.h>

#include <algorithm>

#include <zle/display.h>
#include <zle/keymap.h>

namespace myzsh {
namespace {

TEST(KeySequence, Parse) {
  EXPECT_EQ(parse_key_sequence("^A"), "\x01");
  EXPECT_EQ(parse_key_sequence("^?"), "\x7f");
  EXPECT_EQ(parse_key_sequence("\\e"), "\x1b");
  EXPECT_EQ(parse_key_sequence("^[[A"), "\x1b[A");
  EXPECT_EQ(parse_key_sequence("\\M-a"), "\x1b" "a");
  EXPECT_EQ(parse_key_sequence("\\C-x"), "\x18");
  EXPECT_EQ(parse_key_sequence("\\033"), "\x1b");
  EXPECT_EQ(parse_key_sequence("\\x41"), "A");
  EXPECT_EQ(parse_key_sequence("ab"), "ab");
}

TEST(KeySequence, Format) {
  EXPECT_EQ(format_key_sequence("\x01"), "^A");
  EXPECT_EQ(format_key_sequence("\x1b[A"), "^[[A");
  EXPECT_EQ(format_key_sequence("\x7f"), "^?");
  EXPECT_EQ(format_key_sequence("a"), "a");
}

TEST(Keymap, BindLookupUnbind) {
  Keymap keymap;
  keymap.bind("\x01", {"beginning-of-line", false});
  keymap.bind("\x1b[A", {"up-line-or-history", false});
  EXPECT_EQ(keymap.lookup("\x01"),
            (KeyBinding{"beginning-of-line", false}));
  EXPECT_TRUE(keymap.is_prefix("\x1b"));
  EXPECT_TRUE(keymap.is_prefix("\x1b["));
  EXPECT_FALSE(keymap.is_prefix("\x1b[A"));
  EXPECT_FALSE(keymap.lookup("\x1b"));
  EXPECT_TRUE(keymap.unbind("\x01"));
  EXPECT_FALSE(keymap.lookup("\x01"));
}

TEST(Keymap, Macros) {
  Keymap keymap;
  keymap.bind("\x18" "a", {"echo hi\n", true});
  auto binding = keymap.lookup("\x18" "a");
  ASSERT_TRUE(binding);
  EXPECT_TRUE(binding->is_macro);
}

TEST(Keymap, BindingsSorted) {
  Keymap keymap;
  keymap.bind("b", {"x", false});
  keymap.bind("a", {"y", false});
  auto list = keymap.bindings();
  ASSERT_EQ(list.size(), 2u);
  EXPECT_EQ(list[0].first, "a");
}

TEST(DefaultKeymaps, Emacs) {
  Keymap emacs = make_emacs_keymap();
  EXPECT_EQ(emacs.lookup("\x01")->target, "beginning-of-line");
  EXPECT_EQ(emacs.lookup("\x05")->target, "end-of-line");
  EXPECT_EQ(emacs.lookup("\x0b")->target, "kill-line");
  EXPECT_EQ(emacs.lookup("\x17")->target, "backward-kill-word");
  EXPECT_EQ(emacs.lookup("\x19")->target, "yank");
  EXPECT_EQ(emacs.lookup("\x12")->target,
            "history-incremental-search-backward");
  EXPECT_EQ(emacs.lookup("\r")->target, "accept-line");
  EXPECT_EQ(emacs.lookup("\t")->target, "expand-or-complete");
  EXPECT_EQ(emacs.lookup("\x1b" "f")->target, "forward-word");
  EXPECT_EQ(emacs.lookup("\x1b[A")->target, "up-line-or-history");
  EXPECT_EQ(emacs.fallback_widget, "self-insert");
}

TEST(DefaultKeymaps, Vi) {
  Keymap viins = make_viins_keymap();
  EXPECT_EQ(viins.lookup("\x1b")->target, "vi-cmd-mode");
  EXPECT_EQ(viins.fallback_widget, "self-insert");
  Keymap vicmd = make_vicmd_keymap();
  EXPECT_EQ(vicmd.lookup("h")->target, "vi-backward-char");
  EXPECT_EQ(vicmd.lookup("i")->target, "vi-insert");
  EXPECT_EQ(vicmd.lookup("dd")->target, "vi-kill-whole-line");
  EXPECT_TRUE(vicmd.is_prefix("d"));
}

TEST(KeymapSet, NamesSelectAndLink) {
  KeymapSet keymaps;
  keymaps.create("emacs", make_emacs_keymap());
  keymaps.create("viins", make_viins_keymap());
  ASSERT_TRUE(keymaps.link("emacs", "main"));
  ASSERT_TRUE(keymaps.get("main"));
  EXPECT_EQ(&keymaps.get("main")->get(), &keymaps.get("emacs")->get());
  keymaps.select("viins");
  EXPECT_EQ(keymaps.selected(), "viins");
  vec<str> names = keymaps.names();
  EXPECT_TRUE(s::ranges::is_sorted(names));
  EXPECT_FALSE(keymaps.get("nosuch"));
}

TEST(Display, SingleLine) {
  ScreenLayout layout = layout_line("% ", 2, "echo hi", 7, 80, 0);
  ASSERT_EQ(layout.rows.size(), 1u);
  EXPECT_EQ(layout.rows[0], "% echo hi");
  EXPECT_EQ(layout.cursor_row, 0);
  EXPECT_EQ(layout.cursor_column, 9);
}

TEST(Display, Wrapping) {
  ScreenLayout layout = layout_line("> ", 2, "abcdefgh", 8, 5, 0);
  ASSERT_EQ(layout.rows.size(), 3u);
  EXPECT_EQ(layout.rows[0], "> abc");
  EXPECT_EQ(layout.rows[1], "defgh");
  EXPECT_EQ(layout.cursor_row, 2);
  EXPECT_EQ(layout.cursor_column, 0);
}

TEST(Display, MultiLineBufferAndControlChars) {
  ScreenLayout layout = layout_line("", 0, "a\nb\x01", 4, 80, 0);
  ASSERT_EQ(layout.rows.size(), 2u);
  EXPECT_EQ(layout.rows[1], "b^A");
  EXPECT_EQ(layout.cursor_column, 3);
}

TEST(Display, RightPromptOnlyWhenItFits) {
  EXPECT_TRUE(layout_line("% ", 2, "ls", 2, 80, 10).show_rprompt);
  EXPECT_FALSE(layout_line("% ", 2, str(75, 'x'), 75, 80, 10).show_rprompt);
}

TEST(Display, RenderDiffIsNonEmptyForChanges) {
  ScreenLayout before = layout_line("% ", 2, "a", 1, 80, 0);
  ScreenLayout after = layout_line("% ", 2, "ab", 2, 80, 0);
  EXPECT_FALSE(render_diff(before, after).empty());
}

}  // namespace
}  // namespace myzsh
