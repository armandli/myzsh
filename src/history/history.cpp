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
  // TODO(impl)
  todo("History::read_file");
}

bool History::write_file(strv path, size_t limit, bool append,
                         const HistoryOptions& options) const
{
  // TODO(impl): write to a temp file and rename unless append
  // (HIST_SAVE_BY_COPY).
  todo("History::write_file");
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
