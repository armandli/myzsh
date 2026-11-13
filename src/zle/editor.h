#ifndef EDITOR_H
#define EDITOR_H

#include <functional>

#include <util/aliases.h>
#include <zle/buffer.h>
#include <zle/keymap.h>
#include <zle/terminal.h>

namespace myzsh {

struct Shell;
struct LineEditor;

using WidgetFunction = s::function<bool(LineEditor&)>;

// The Zsh Line Editor (Src/Zle/zle_main.c). Input comes from a key source
// so tests can feed keys without a terminal; the interactive loop uses
// Terminal::read_byte.
struct LineEditor {
  using KeySource = s::function<KeyRead(int timeout_ms)>;

  explicit LineEditor(Shell& shell) : mShell(shell) {}

  // Creates the default keymaps and registers the builtin widgets:
  //   movement: forward-char backward-char forward-word backward-word
  //     beginning-of-line end-of-line vi-* motions
  //   editing: self-insert delete-char backward-delete-char kill-line
  //     backward-kill-line kill-whole-line kill-word backward-kill-word
  //     yank yank-pop transpose-chars transpose-words up/down/capitalize-word
  //     quoted-insert overwrite-mode undo redo set-mark-command
  //     exchange-point-and-mark kill-region copy-region-as-kill
  //   history: up-line-or-history down-line-or-history history-search-backward
  //     history-incremental-search-backward/forward beginning/end-of-history
  //     insert-last-word infer-next-history
  //   misc: accept-line accept-and-hold push-line get-line clear-screen
  //     send-break vi-cmd-mode vi-insert vi-add-next ... expand-or-complete
  //     complete-word list-choices menu-complete delete-char-or-list
  //     run-help which-command expand-history magic-space
  //     bracketed-paste (bound to "\e[200~": inserts up to "\e[201~"
  //     literally, never accepting the line)
  void init();

  // Reads one line. nullopt on EOF (^D on an empty line, or the source
  // reporting EndOfFile) or interrupt. Interrupted: runs
  // Shell::dispatch_signals(), which may print job notices (NOTIFY) above
  // the prompt, then redisplays and keeps reading. TimedOut while idle:
  // runs due sched events. send-break (^C) discards the line.
  opt<str> read_line(strv prompt, strv rprompt, KeySource source);
  // SIGWINCH: re-reads the terminal size and redraws prompt and buffer.
  void resize(int columns, int lines);

  // Feeds one complete key sequence as if typed, running the bound widget.
  // Returns false if the editor finished (accept-line, EOF).
  bool process_keys(strv keys);
  bool call_widget(strv name);

  void define_widget(strv name, WidgetFunction function);
  // zle -N name func: a user widget implemented by a shell function, run
  // with BUFFER/CURSOR/LBUFFER/RBUFFER/... special parameters bound.
  void define_user_widget(strv name, strv function_name);
  vec<str> widget_names() const;

  LineBuffer& buffer() { return mBuffer; }
  KeymapSet& keymaps() { return mKeymaps; }
  bool finished() const { return mFinished; }
  bool eof() const { return mEof; }
  // Current position while walking history with up/down-line-or-history.
  int history_index() const { return mHistoryIndex; }

protected:
  bool history_move(int delta);

private:
  Shell& mShell;
  LineBuffer mBuffer;
  KeymapSet mKeymaps;
  umap<str, WidgetFunction> mWidgets;
  str mLastWidget;
  int mHistoryIndex = 0;
  // Buffer contents saved when the user starts walking history.
  str mSavedLine;
  // Lines saved by push-line, restored after the next accept.
  vec<str> mBufferStack;
  bool mFinished = false;
  bool mEof = false;
};

}  // namespace myzsh

#endif  // EDITOR_H
