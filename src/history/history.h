#ifndef HISTORY_H
#define HISTORY_H

#include <cstdint>
#include <expected>

#include <util/aliases.h>

namespace myzsh {

struct HistoryEntry {
  int number = 0;
  str text;
  s::int64_t start_time = 0;
  // Seconds the command took (EXTENDED_HISTORY), 0 if unknown.
  s::int64_t duration = 0;
};

struct HistoryOptions {
  bool ignore_dups = false;      // HIST_IGNORE_DUPS
  bool ignore_all_dups = false;  // HIST_IGNORE_ALL_DUPS
  bool ignore_space = false;     // HIST_IGNORE_SPACE
  bool reduce_blanks = false;    // HIST_REDUCE_BLANKS
  bool no_functions = false;     // HIST_NO_FUNCTIONS
  bool no_store = false;         // HIST_NO_STORE (history/fc -l not saved)
  bool extended = false;         // EXTENDED_HISTORY file format
  bool save_no_dups = false;     // HIST_SAVE_NO_DUPS
  bool expire_dups_first = false; // HIST_EXPIRE_DUPS_FIRST
  bool fcntl_lock = false;       // HIST_FCNTL_LOCK (else a $HISTFILE.LOCK file)
  bool save_by_copy = true;      // HIST_SAVE_BY_COPY (rewrite via temp + rename)
};

struct HistoryExpansion {
  str line;
  // The line contained history references (it must be echoed).
  bool changed = false;
  // :p modifier: print the result instead of running it.
  bool print_only = false;
};

// Command history (Src/hist.c).
struct History {
  // Returns false if the line was not stored (ignored by options).
  // HIST_IGNORE_ALL_DUPS removes the older duplicate and stores the new
  // line.
  bool add(strv line, s::int64_t time, const HistoryOptions& options);
  // nullopt for numbers outside [first_number(), next_number()).
  opt<s::reference_wrapper<const HistoryEntry>> get(int number) const;
  // Event number the next added line will receive ($HISTCMD + 1).
  int next_number() const { return mNextNumber; }
  int first_number() const;
  size_t size() const { return mEntries.size(); }
  const vec<HistoryEntry>& entries() const { return mEntries; }

  // HISTSIZE: oldest entries dropped beyond the limit.
  void set_limit(size_t limit);

  // Most recent event whose text starts with / contains `text`, searching
  // backwards from `before` (exclusive).
  opt<int> search_prefix(strv text, int before) const;
  opt<int> search_substring(strv text, int before) const;

  // History file I/O (TODO.md §2.3.10). The object remembers which
  // entries are already in the file (saved_up_to()) and how big the file
  // was when last read or written, so appending never writes an entry
  // twice — writing everything in append mode would double the file every
  // session. Every write holds the file lock.
  //
  // Reads plain ("cmd") or extended (": 1700000000:0;cmd") format lines; a
  // trailing backslash continues the entry onto the next line. Entries read
  // count as saved. Used at startup and by `fc -R`.
  bool read_file(strv path);
  // Reads only what other shells appended since this object last read or
  // wrote the file (SHARE_HISTORY, before each prompt). The new entries
  // count as saved.
  bool read_new(strv path);
  // Appends the entries not yet saved, then marks them saved:
  // INC_APPEND_HISTORY after each command, APPEND_HISTORY on exit, `fc -A`.
  bool append_new(strv path, const HistoryOptions& options);
  // Replaces the file with the last `limit` (SAVEHIST) entries, through a
  // temp file and rename() when options.save_by_copy; marks every entry
  // saved. Used on exit without APPEND_HISTORY, and `fc -W`.
  bool rewrite(strv path, size_t limit, const HistoryOptions& options);
  // Event number of the first entry not yet in the file.
  int saved_up_to() const { return mSavedUpTo; }

  // Csh-style history expansion with `bang` (default '!') and `hat` ('^'):
  //   !! !n !-n !str !?str[?] !# !{...}  ^old^new[^]
  //   word designators :0 :n :^ :$ :% :x-y :* :x* :x-
  //   modifiers :h :t :r :e :s/l/r/ :gs :& :p :q :x :u :l
  // Errors: "event not found: X", "no such word in event", etc.
  s::expected<HistoryExpansion, str> expand(strv line, char bang = '!',
                                            char hat = '^') const;

private:
  vec<HistoryEntry> mEntries;
  int mNextNumber = 1;
  size_t mLimit = 30;
  int mSavedUpTo = 1;
  // The file as last read or written, to find other shells' additions.
  s::int64_t mFileSize = 0;
  s::int64_t mFileMtime = 0;
};

// Splits a command line into shell words the way history word designators
// count them (quotes kept, operators like | and ; are separate words).
vec<str> split_history_words(strv line);

}  // namespace myzsh

#endif  // HISTORY_H
