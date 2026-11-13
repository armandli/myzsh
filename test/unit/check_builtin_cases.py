#!/usr/bin/env python3
"""Validates the CASES table in builtins_test.cpp against a real zsh.

Usage: python3 test/unit/check_builtin_cases.py [path/to/zsh]
"""
import os
import re
import subprocess
import sys
import tempfile

STRING = r'(?:R"x\((?P<{0}raw>.*?)\)x"|"(?P<{0}lit>(?:[^"\\]|\\.)*)")'
CASE = re.compile(
    r'\{"(?P<name>\w+)",\s*' + STRING.format("s") + r',\s*' +
    STRING.format("o") + r',\s*(?P<status>\d+)\}', re.S)


def c_unescape(text):
    return bytes(text, "utf-8").decode("unicode_escape")


def value(match, prefix):
    raw = match.group(prefix + "raw")
    return raw if raw is not None else c_unescape(match.group(prefix + "lit"))


def main():
    zsh = sys.argv[1] if len(sys.argv) > 1 else "zsh"
    source = open(os.path.join(os.path.dirname(__file__),
                               "builtins_test.cpp")).read()
    failures = 0
    count = 0
    for match in CASE.finditer(source):
        count += 1
        script = value(match, "s")
        expected = value(match, "o")
        with tempfile.TemporaryDirectory() as cwd:
            env = dict(os.environ, PATH="/bin:/usr/bin")
            done = subprocess.run([zsh, "-f", "-c", script], cwd=cwd, env=env,
                                  capture_output=True, text=True,
                                  stdin=subprocess.DEVNULL, timeout=20)
        status = int(match.group("status"))
        if done.stdout != expected or done.returncode != status:
            failures += 1
            print(f"MISMATCH {match.group('name')}: status {done.returncode} "
                  f"(want {status})\n  got:  {done.stdout!r}\n"
                  f"  want: {expected!r}\n  err:  {done.stderr!r}")
    print(f"{count} cases, {failures} mismatches")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
