#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include <functional>

#include <shell/shell.h>
#include <util/aliases.h>

namespace myzsh::test {

// Temporary directory removed (recursively) on destruction.
struct TempDir {
  TempDir();
  ~TempDir();
  TempDir(const TempDir&) = delete;
  TempDir& operator=(const TempDir&) = delete;

  const sf::path& path() const { return mPath; }
  sf::path operator/(strv name) const { return mPath / name; }
  // Creates a file (and parent directories) with the given contents.
  sf::path write(strv relative, strv contents = "") const;
  sf::path mkdir(strv relative) const;

private:
  sf::path mPath;
};

// Changes the working directory for the lifetime of the object.
struct ScopedCwd {
  explicit ScopedCwd(const sf::path& directory);
  ~ScopedCwd();

private:
  sf::path mPrevious;
};

struct Captured {
  int status = 0;
  str out;
  str err;
};

// Runs fn with fds 1 and 2 redirected to temporary files and returns what
// was written. Works for output from forked children too.
Captured capture(const s::function<int()>& fn);

// Shell set up with init_for_test(), so no rc files or environment leak in.
uptr<Shell> make_shell();

// shell.run_string(script) with output captured.
Captured run(Shell& shell, strv script);
// Convenience for one-off scripts in a fresh shell.
Captured run(strv script);

str read_file(const sf::path& path);

}  // namespace myzsh::test

#endif  // TEST_SUPPORT_H
