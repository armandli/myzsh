#ifndef QSTRING_H
#define QSTRING_H

#include <util/aliases.h>

namespace myzsh {

// A string that remembers which characters were quoted in the source word.
// It replaces zsh's in-band token bytes (Star, Quest, Snull, ...): pattern
// and brace code only treat unquoted characters as special.
struct QString {
  str text;
  vec<bool> quoted;

  QString() = default;
  explicit QString(strv unquoted_text)
    : text(unquoted_text), quoted(unquoted_text.size(), false) {}

  static QString literal(strv quoted_text) {
    QString result;
    result.append(quoted_text, true);
    return result;
  }

  void append(strv piece, bool is_quoted) {
    text.append(piece);
    quoted.insert(quoted.end(), piece.size(), is_quoted);
  }
  void append(const QString& other) {
    text.append(other.text);
    quoted.insert(quoted.end(), other.quoted.begin(), other.quoted.end());
  }
  bool is_quoted(size_t index) const { return quoted.at(index); }
  size_t size() const { return text.size(); }
  bool empty() const { return text.empty(); }

  bool operator==(const QString&) const = default;
};

}  // namespace myzsh

#endif  // QSTRING_H
