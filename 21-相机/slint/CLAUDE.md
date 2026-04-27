# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Repository Is

This is **not** a source code project. It is a directory containing pre-built third-party libraries and binaries used as dependencies by other projects.

## Contents

### Slint C++ SDK (v1.15.1, Linux x86_64)

Located at `Slint-cpp-1.15.1-Linux-x86_64/`. This is a pre-built binary distribution of the [Slint](https://slint.dev/) UI toolkit for C++.

- `bin/slint-compiler` — The Slint `.slint` → C++ header code generator
- `include/slint/` — C++ headers (main entry point: `slint.h`)
- `lib/libslint_cpp.so` — Shared library
- `lib/cmake/Slint/` — CMake package config files

### Using Slint from a CMake project

```cmake
set(CMAKE_PREFIX_PATH "/home/zcli/BIN-LIB/Slint-cpp-1.15.1-Linux-x86_64")
find_package(Slint REQUIRED)
target_link_libraries(my_target PRIVATE Slint::Slint)
slint_target_sources(my_target my_ui.slint)
```

Key CMake options:
- `SLINT_STYLE` — Widget style (default: `qt`)
- `SLINT_EMBED_RESOURCES` — Resource embedding mode (`as-absolute-path`, `embed-files`, `embed-for-software-renderer`)
- `SLINT_TARGET_SOURCES(target file.slint NAMESPACE ns)` — Compile `.slint` files and generate C++ headers
