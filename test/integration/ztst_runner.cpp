#include <integration/ztst_runner.h>

#include <fcntl.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

#include <support/test_support.h>

extern char** environ;

namespace myzsh::ztst {

namespace {

enum class Section : int {
  None,
  Prep,
  Test,
  Clean,
};

str join_lines(const vec<str>& lines) {
  str result;
  for (const str& line : lines) {
    result += line;
    result += '\n';
  }
  return result;
}

str strip_indent(strv line) {
  size_t skip = 0;
  while (skip < 2 and skip < line.size() and line[skip] == ' ') {
    ++skip;
  }
  return str(line.substr(skip));
}

// "0:desc", "-:desc", "1f:desc", "0dD:desc", "0s:desc", "1i:desc"
bool parse_status_line(strv line, Case& test_case) {
  size_t colon = line.find(':');
  if (colon == strv::npos or colon == 0) {
    return false;
  }
  strv head = line.substr(0, colon);
  size_t digits = 0;
  while (digits < head.size() and s::isdigit(head[digits])) {
    ++digits;
  }
  if (digits == 0 and head[0] != '-') {
    return false;
  }
  if (digits > 0) {
    test_case.status = s::stoi(str(head.substr(0, digits)));
  } else {
    test_case.status = s::nullopt;
    digits = 1;
  }
  for (char flag : head.substr(digits)) {
    switch (flag) {
    case 'd': test_case.check_out = false;
    break; case 'D': test_case.check_err = false;
    break; case 'f': test_case.expected_failure = true;
    break; case 's': case 'i':
      if (test_case.mode != Mode::Command) {
        return false;
      }
      test_case.mode = flag == 's' ? Mode::Script : Mode::Stdin;
    break; default: return false;
    }
  }
  test_case.description = str(line.substr(colon + 1));
  return true;
}

str read_all(int fd) {
  str result;
  char buffer[4096];
  lseek(fd, 0, SEEK_SET);
  ssize_t count;
  while ((count = read(fd, buffer, sizeof buffer)) > 0) {
    result.append(buffer, static_cast<size_t>(count));
  }
  return result;
}

int temp_fd(const test::TempDir& dir, strv name, strv contents) {
  sf::path path = dir.write(name, contents);
  return open(path.c_str(), O_RDWR);
}

str replace_all(str text, strv from, strv to) {
  for (size_t at = text.find(from); at != str::npos;
       at = text.find(from, at + to.size())) {
    text.replace(at, from.size(), to);
  }
  return text;
}

}  // namespace

vec<Case> parse_file(const sf::path& path) {
  s::ifstream in(path);
  if (not in) {
    throw s::runtime_error("cannot read " + path.string());
  }
  vec<Case> cases;
  Section section = Section::None;
  vec<str> prep;
  vec<str> code;
  vec<str> input;
  vec<str> out;
  vec<str> err;
  Case current;
  bool have_status = false;

  auto flush = [&] {
    if (not code.empty() and have_status) {
      current.file = path.stem().string();
      current.index = static_cast<int>(cases.size()) + 1;
      current.prep = join_lines(prep);
      current.code = join_lines(code);
      current.input = join_lines(input);
      current.out = join_lines(out);
      current.err = join_lines(err);
      cases.push_back(current);
    } else if (not code.empty()) {
      throw s::runtime_error(path.string() + ": test without status line: " +
                             code.front());
    }
    code.clear();
    input.clear();
    out.clear();
    err.clear();
    current = Case{};
    have_status = false;
  };

  str line;
  while (s::getline(in, line)) {
    if (line.starts_with("%prep")) {
      section = Section::Prep;
      continue;
    }
    if (line.starts_with("%test")) {
      section = Section::Test;
      continue;
    }
    if (line.starts_with("%clean")) {
      flush();
      section = Section::Clean;
      continue;
    }
    if (line.starts_with("#")) {
      continue;
    }
    if (section == Section::Prep) {
      if (not line.empty()) {
        prep.push_back(strip_indent(line));
      }
      continue;
    }
    if (section != Section::Test) {
      continue;
    }
    if (line.empty()) {
      if (have_status) {
        flush();
      } else if (not code.empty()) {
        code.push_back("");
      }
      continue;
    }
    if (line.starts_with("  ")) {
      if (have_status) {
        flush();
      }
      code.push_back(strip_indent(line));
    } else if (line[0] == '<') {
      input.push_back(line.substr(1));
    } else if (line[0] == '>') {
      out.push_back(line.substr(1));
    } else if (line[0] == '?') {
      err.push_back(line.substr(1));
    } else if (line.starts_with("F:")) {
      current.note += line.substr(2) + "\n";
    } else if (not have_status and parse_status_line(line, current)) {
      have_status = true;
    } else {
      throw s::runtime_error(path.string() + ": unexpected line: " + line);
    }
  }
  flush();
  return cases;
}

vec<Case> load_directory(const sf::path& directory) {
  vec<sf::path> files;
  for (const auto& entry : sf::directory_iterator(directory)) {
    if (entry.path().extension() == ".ztst") {
      files.push_back(entry.path());
    }
  }
  s::ranges::sort(files);
  vec<Case> all;
  for (const sf::path& file : files) {
    vec<Case> cases = parse_file(file);
    all.insert(all.end(), cases.begin(), cases.end());
  }
  return all;
}

Outcome run_case(const Case& test_case, const sf::path& shell,
                 int timeout_seconds)
{
  test::TempDir dir;
  // Script mode: the script sits in its own directory so the test's
  // working directory holds only what the test creates.
  test::TempDir script_dir;
  str script = test_case.prep + test_case.code;
  str stdin_text = test_case.mode == Mode::Stdin ? script + test_case.input
                                                 : test_case.input;
  int in_fd = temp_fd(dir, ".ztst-stdin", stdin_text);
  int out_fd = temp_fd(dir, ".ztst-stdout", "");
  int err_fd = temp_fd(dir, ".ztst-stderr", "");
  sf::remove(dir / ".ztst-stdin");
  sf::remove(dir / ".ztst-stdout");
  sf::remove(dir / ".ztst-stderr");

  str shell_path = shell.string();
  str home = "HOME=" + dir.path().string();
  vec<str> env_strings{"PATH=/bin:/usr/bin", home, "LC_ALL=C", "TERM=xterm-256color",
                       "USER=" + str(getenv("USER") ? getenv("USER") : "")};
  vec<char*> envp;
  for (str& entry : env_strings) {
    envp.push_back(entry.data());
  }
  envp.push_back(nullptr);
  str flag_f = "-f";
  str flag_c = "-c";
  str script_path = (script_dir / "case.zsh").string();
  vec<char*> argv{shell_path.data(), flag_f.data()};
  switch (test_case.mode) {
  case Mode::Command:
    argv.push_back(flag_c.data());
    argv.push_back(script.data());
  break; case Mode::Script:
    script_dir.write("case.zsh", script);
    argv.push_back(script_path.data());
  break; case Mode::Stdin:
  break; default:
    break;
  }
  argv.push_back(nullptr);

  posix_spawn_file_actions_t actions;
  posix_spawn_file_actions_init(&actions);
  posix_spawn_file_actions_adddup2(&actions, in_fd, 0);
  posix_spawn_file_actions_adddup2(&actions, out_fd, 1);
  posix_spawn_file_actions_adddup2(&actions, err_fd, 2);
  posix_spawn_file_actions_addchdir(&actions, dir.path().c_str());
  posix_spawnattr_t attributes;
  posix_spawnattr_init(&attributes);
  // Own process group so a runaway test (and its children) can be killed.
  posix_spawnattr_setflags(&attributes, POSIX_SPAWN_SETPGROUP);
  posix_spawnattr_setpgroup(&attributes, 0);

  pid_t pid = 0;
  int spawned = posix_spawn(&pid, shell_path.c_str(), &actions, &attributes,
                            argv.data(), envp.data());
  posix_spawn_file_actions_destroy(&actions);
  posix_spawnattr_destroy(&attributes);
  if (spawned != 0) {
    throw s::runtime_error("cannot run " + shell_path);
  }

  Outcome outcome;
  int wait_status = 0;
  auto deadline = sc::steady_clock::now() + sc::seconds(timeout_seconds);
  while (waitpid(pid, &wait_status, WNOHANG) == 0) {
    if (sc::steady_clock::now() > deadline) {
      kill(-pid, SIGKILL);
      waitpid(pid, &wait_status, 0);
      outcome.timed_out = true;
      break;
    }
    st::sleep_for(sc::milliseconds(5));
  }
  if (WIFEXITED(wait_status)) {
    outcome.status = WEXITSTATUS(wait_status);
  } else if (WIFSIGNALED(wait_status)) {
    outcome.status = 128 + WTERMSIG(wait_status);
  }
  outcome.out = read_all(out_fd);
  outcome.err = read_all(err_fd);
  close(in_fd);
  close(out_fd);
  close(err_fd);

  if (test_case.mode == Mode::Script) {
    outcome.out = replace_all(outcome.out, script_path, "case.zsh");
    outcome.err = replace_all(outcome.err, script_path, "case.zsh");
  }
  str prefix = shell.filename().string() + ":";
  if (prefix != "zsh:") {
    s::regex leading("(^|\n)" + s::regex_replace(
      prefix, s::regex(R"([.^$|()\[\]{}*+?\\])"), R"(\$&)"));
    outcome.err = s::regex_replace(outcome.err, leading, "$1zsh:");
  }
  return outcome;
}

str case_name(const Case& test_case) {
  str name = test_case.file + "_" + s::to_string(test_case.index) + "_";
  for (char c : test_case.description) {
    name += s::isalnum(static_cast<unsigned char>(c)) ? c : '_';
  }
  return name.substr(0, 100);
}

}  // namespace myzsh::ztst
