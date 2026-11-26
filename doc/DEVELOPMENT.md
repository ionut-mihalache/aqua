# Development Guide

This guide covers everything you need to build, test and change AQUA. For the contribution workflow, see [CONTRIBUTING.md](../CONTRIBUTING.md).

## Project layout

```
aqua
├── core/
│   ├── client/        # Client side: connect, call, client info
│   └── service/       # Service side: install, call handling, return, service info
├── include/           # Public and shared headers (DSP, protocol, types, sync)
├── platform/
│   ├── include/       # Platform abstraction interface (platform.h)
│   ├── linux/         # Linux implementation
│   └── windows/       # Windows implementation
├── utils/             # Common helpers
├── initializer/       # Small program that initialises the service area (calls initService)
├── containers/        # Development containers (Ubuntu 24.04, Fedora 44)
├── test/              # Example clients/services and benchmarks (C++, Java, PHP)
└── doc/               # Documentation
```

**`core/` must stay platform-agnostic.** Anything OS-specific (memory mapping, shared memory objects, synchronisation, processes, allocation) goes behind the interface in `platform/include/platform.h`, with one implementation per platform under `platform/<os>/`. If you add or change a platform function, implement it for both Linux and Windows. If you can only test on one, say so in your pull request.

## Requirements

- A C23-capable compiler: GCC or Clang on Linux; MSVC (Visual Studio 2022) or Clang on Windows
- CMake **3.25.1** or newer
- Ninja (recommended on Windows)
- `clang-format`, and optionally `clangd` (a `compile_commands.json` is generated in `build/`)

## Development container (Linux, recommended)

The repository includes containers with the full toolchain needed to build the library and run the examples and benchmarks (GCC, Clang, clangd, CMake, Java 21, PHP with ZeroMQ, Python):

```bash
cd containers/ubuntu24.04      # or containers/fedora44
docker compose up -d --build
docker exec -it aqua bash
cd /root/aqua
```

Notes:

- The compose file mounts the repository from `../../../aqua`, so your clone must be in a directory named **`aqua`**.
- The host directory `/shared-mem` is mounted as `/dev/shm` in the container. Create it on the host first.
- Your `~/.ssh` is mounted read-only, so git over SSH works inside the container.

## Building

Run all commands from the repository root.

**Linux:**

```bash
rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
```

**Windows:**

```bash
rm -rf build
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Use `Debug` while developing. It disables optimisation, adds debug info and defines `__AQUA_DEBUG__`. Use `Release` to measure performance.

The libraries are written to `build/lib/shared/` and `build/lib/static/`.

> **Warnings are errors:** `-Wall -Wextra -Werror` on Linux, `/W4 /WX` on Windows.

## Testing

There is no automated unit-test suite yet. **Contributions that add one are very welcome.** Until then, verify your change by:

1. **Building in both `Debug` and `Release`** without warnings.
2. **Running an example** from `test/` that exercises the code you changed. For example, the C++ client:

   ```bash
   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/build/lib/shared
   cd test/cpp/client
   make && make run
   ```

   Java (`test/java/service/xslt-transformation`) and PHP (`test/php/client`) examples are also available.
3. **Checking for memory and concurrency errors** where relevant, with Valgrind or with `-fsanitize=address` / `-fsanitize=thread`.
4. **Running the benchmarks** in `test/php/client/` if your change touches a hot path (queues, synchronisation, call/return), and including before/after numbers in your pull request.

CI (GitHub Actions) builds the library on Linux and Windows for every pull request.

## Coding conventions

### Formatting

Code follows the repository's `.clang-format` (LLVM base, 4-space indentation). Format **only the code you changed**:

```bash
git clang-format                 # formats lines changed since the last commit
clang-format -i path/to/file.c   # formats a whole file
```

Please don't reformat unrelated code in the same pull request.

### Files

- Every `.c` and `.h` file starts with:

  ```c
  // SPDX-License-Identifier: LGPL-2.1-or-later
  ```

- Header guards use the form `__AQUA_<NAME>_H_`.
- Includes are grouped and sorted: the file's own header, then project headers, then system headers.
- Add new source files to `CORE_SOURCES` or the platform's `PLATFORM_SOURCES` in `CMakeLists.txt`.

### Naming

| Kind                            | Convention                                          | Example                      |
|---------------------------------|-----------------------------------------------------|------------------------------|
| Exported public API             | `aqua` prefix, camelCase, marked `AQUA_API_EXPORT`  | `aquaInstall`                |
| Internal (non-static) functions | camelCase                                           | `initService`                |
| `static` functions              | `sf_` prefix                                | `s_QPop`, `sf_GetServiceOff` |
| `static` variables              | `s_` prefix                                         | `s_ClientInfo`               |
| Function parameters             | `p_` prefix                                         | `p_Size`, `p_Q`              |
| Struct members                  | `m_` prefix                                         | `m_Metadata`, `m_Size`       |

### General

- Symbols are hidden by default (`-fvisibility=hidden`). Export only what library users need.
- Check every return value from platform calls and propagate errors.
- Keep shared-memory layouts explicit and stable: mind alignment, use fixed-width types (`int32_t`, `aqua_size_t`), and remember that clients in other languages read the same memory.
- Comment *why* something is done, especially around synchronisation and memory ordering.
