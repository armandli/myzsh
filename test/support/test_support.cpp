#include <support/test_support.h>

#include <fcntl.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace myzsh::test {

namespace {

sf::path temp_root() {
  const char* dir = s::getenv("TMPDIR");
  return dir != nullptr ? sf::path(dir) : sf::path("/tmp");
}

struct FdRedirect {
  int target;
  int saved;

  FdRedirect(int fd, int replacement) : target(fd), saved(dup(fd)) {
    s::fflush(nullptr);
    dup2(replacement, fd);
  }
  ~FdRedirect() {
    s::fflush(nullptr);
    dup2(saved, target);
    close(saved);
  }
};

}  // namespace

TempDir::TempDir() {
  str pattern = (temp_root() / "myzsh-test-XXXXXX").string();
  if (mkdtemp(pattern.data()) == nullptr) {
    throw s::runtime_error("mkdtemp failed");
  }
  // Canonical so tests comparing $PWD against it are not fooled by
  // /tmp -> /private/tmp style symlinks.
  mPath = sf::canonical(pattern);
}

TempDir::~TempDir() {
  s::error_code ignored;
  sf::remove_all(mPath, ignored);
}

sf::path TempDir::write(strv relative, strv contents) const {
  sf::path target = mPath / relative;
  sf::create_directories(target.parent_path());
  s::ofstream out(target, s::ios::binary);
  out << contents;
  return target;
}

sf::path TempDir::mkdir(strv relative) const {
  sf::path target = mPath / relative;
  sf::create_directories(target);
  return target;
}

ScopedCwd::ScopedCwd(const sf::path& directory)
  : mPrevious(sf::current_path())
{
  sf::current_path(directory);
}

ScopedCwd::~ScopedCwd() {
  s::error_code ignored;
  sf::current_path(mPrevious, ignored);
}

str read_file(const sf::path& path) {
  s::ifstream in(path, s::ios::binary);
  s::ostringstream buffer;
  buffer << in.rdbuf();
  return buffer.str();
}

Captured capture(const s::function<int()>& fn) {
  TempDir dir;
  sf::path out_path = dir / "stdout";
  sf::path err_path = dir / "stderr";
  int out_fd = open(out_path.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0600);
  int err_fd = open(err_path.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0600);
  Captured result;
  {
    FdRedirect out(1, out_fd);
    FdRedirect err(2, err_fd);
    try {
      result.status = fn();
    } catch (...) {
      close(out_fd);
      close(err_fd);
      throw;
    }
  }
  close(out_fd);
  close(err_fd);
  result.out = read_file(out_path);
  result.err = read_file(err_path);
  return result;
}

uptr<Shell> make_shell() {
  auto shell = s::make_unique<Shell>();
  shell->init_for_test();
  return shell;
}

Captured run(Shell& shell, strv script) {
  return capture([&] { return shell.run_string(script); });
}

Captured run(strv script) {
  auto shell = make_shell();
  return run(*shell, script);
}

}  // namespace myzsh::test
