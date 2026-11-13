# myzsh
building a zsh for fun

A reimplementation of zsh 5.9 in C++26. See [TODO.md](TODO.md) for the
architecture and implementation plan.

```
make build        # cmake configure + build into build/
make test         # unit + integration tests
make reference    # validate integration expectations against the real zsh
make run          # start build/src/myzsh
```

Requires CMake >= 3.28, a C++26 compiler (Apple clang 21 / clang 18+ /
GCC 14+) and GoogleTest (found via CMake or downloaded automatically).
