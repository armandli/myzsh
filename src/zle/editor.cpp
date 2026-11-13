#include <zle/editor.h>

#include <shell/shell.h>
#include <util/todo.h>

namespace myzsh {

void LineEditor::init() {
  // TODO(impl)
  todo("LineEditor::init");
}

opt<str> LineEditor::read_line(strv prompt, strv rprompt, KeySource source) {
  // TODO(impl): read bytes, accumulating while the selected keymap reports
  // a prefix (KEYTIMEOUT in hundredths of a second), call process_keys,
  // redraw via layout_line/render_diff after each widget. Handle each
  // KeyRead status as described in editor.h; EndOfFile must end the loop.
  todo("LineEditor::read_line");
}

void LineEditor::resize(int columns, int lines) {
  // TODO(impl): set $COLUMNS/$LINES, re-run layout_line and redraw the
  // whole prompt and buffer (not just the last physical line).
  todo("LineEditor::resize");
}

bool LineEditor::process_keys(strv keys) {
  // TODO(impl): lookup in the selected keymap; unbound printable chars use
  // the fallback widget; macros push their string back into the input.
  todo("LineEditor::process_keys");
}

bool LineEditor::call_widget(strv name) {
  // TODO(impl): wrap with mBuffer.begin_change() for undo grouping.
  todo("LineEditor::call_widget");
}

void LineEditor::define_widget(strv name, WidgetFunction function) {
  // TODO(impl)
  todo("LineEditor::define_widget");
}

void LineEditor::define_user_widget(strv name, strv function_name) {
  // TODO(impl)
  todo("LineEditor::define_user_widget");
}

vec<str> LineEditor::widget_names() const {
  // TODO(impl)
  todo("LineEditor::widget_names");
}

bool LineEditor::history_move(int delta) {
  // TODO(impl)
  todo("LineEditor::history_move");
}

}  // namespace myzsh
