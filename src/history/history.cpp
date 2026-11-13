#include <history/history.h>

#include <util/todo.h>

namespace myzsh {

bool History::add(strv line, s::int64_t time, const HistoryOptions& options) {
  // TODO(impl): strip the trailing newline; skip empty lines.
  todo("History::add");
}

opt<s::reference_wrapper<const HistoryEntry>> History::get(int number) const {
  // TODO(impl)
  todo("History::get");
}

int History::first_number() const {
  // TODO(impl)
  todo("History::first_number");
}

void History::set_limit(size_t limit) {
  // TODO(impl)
  todo("History::set_limit");
}

opt<int> History::search_prefix(strv text, int before) const {
  // TODO(impl)
  todo("History::search_prefix");
}

opt<int> History::search_substring(strv text, int before) const {
  // TODO(impl)
  todo("History::search_substring");
}

bool History::read_file(strv path) {
  // TODO(impl): renumber so the read entries come before any in memory;
  // set mSavedUpTo past them and record size/mtime.
  todo("History::read_file");
}

bool History::read_new(strv path) {
  // TODO(impl): unchanged size/mtime -> nothing. Otherwise read from
  // mFileSize on (if the file shrank, another shell rewrote it: skip, as
  // zsh does, and just record the new size). Lock while reading.
  todo("History::read_new");
}

bool History::append_new(strv path, const HistoryOptions& options) {
  // TODO(impl): lock; open O_APPEND; write entries from mSavedUpTo in the
  // format chosen by options.extended (multi-line entries with backslash
  // continuations); update mSavedUpTo and size/mtime.
  todo("History::append_new");
}

bool History::rewrite(strv path, size_t limit,
                      const HistoryOptions& options)
{
  // TODO(impl): lock; with save_by_copy write "<path>.new" and rename();
  // otherwise truncate in place. save_no_dups drops older duplicates.
  // Everything is saved afterwards.
  todo("History::rewrite");
}

s::expected<HistoryExpansion, str> History::expand(strv line, char bang,
                                                   char hat) const
{
  // TODO(impl): '!' followed by space, tab, newline, '=' or '(' is literal;
  // so is a backslash-escaped or single-quoted '!'. !# is the current line
  // so far.
  todo("History::expand");
}

vec<str> split_history_words(strv line) {
  // TODO(impl)
  todo("split_history_words");
}

}  // namespace myzsh
