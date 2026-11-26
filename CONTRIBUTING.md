# Contributing to AQUA

Thank you for your interest in __AQUA__!

__AQUA__ is open to contributions from anyone: bug reports, ideas, documentation, tests or code.

By participating, you agree to follow our [Code of Conduct](CODE_OF_CONDUCT.md).

## Ways to contribute

- **Report a bug**: [open a bug report](https://github.com/ionut-mihalache/aqua/issues/new/choose)
- **Suggest a feature**: [open a feature request](https://github.com/ionut-mihalache/aqua/issues/new/choose)
- **Improve the documentation**: README, usage examples, a description of the DSP protocol
- **Test on other platforms**: other Linux distributions, Windows toolchains
- **Write clients or services in other languages**: see the C++, Java and PHP examples in `test/`
- **Fix bugs or implement features** in the library

Looking for inspiration? See **[What can you build with AQUA?](doc/USE_CASES.md)** for use cases and project ideas, including bindings and demos the community could build.

Found a security vulnerability? Please **don't** open a public issue. See [SECURITY.md](SECURITY.md).

## Before you start

- Search the [existing issues](https://github.com/ionut-mihalache/aqua/issues) first, to see whether someone has already reported the problem or is working on it.
- For larger changes, especially anything touching the **public headers** or the **DSP protocol layout**, open an issue to discuss the idea **before** writing code.
- Comment on an issue to say you're working on it, so effort isn't duplicated.

## Workflow

1. Fork the repository and clone your fork:

   ```bash
   git clone https://github.com/<your-username>/aqua.git
   cd aqua
   git remote add upstream https://github.com/ionut-mihalache/aqua.git
   ```

2. Create a branch from an up-to-date `main`:

   ```bash
   git checkout main
   git pull upstream main
   git checkout -b fix/short-description
   ```

3. Make your changes, then build and test them. See the **[Development guide](doc/DEVELOPMENT.md)** for setup, build commands, testing and coding conventions.

4. Commit using the project's format:

   ```
   [type](scope): Short summary in imperative mood
   ```

   Use `[new]` for new features, `[update]` for improvements or refactors and `[fix]` for bug fixes. The scope is optional. For example:

   ```
   [fix](platform/linux): Fix lost initial umask value in sf_Create
   ```

5. Push your branch and open a pull request against `main`. The pull request template will guide you through the description.

A maintainer will review your PR. Being asked for changes is normal and part of the process. Push new commits to the same branch and the PR updates automatically.

## Key rules

- The code must compile **with no warnings** (warnings are treated as errors) on Linux and Windows. CI checks both.
- `core/` stays platform-agnostic. OS-specific code goes in `platform/` and must be implemented for **both** Linux and Windows.
- Format only the code you changed, with `clang-format`.
- Every source file starts with `// SPDX-License-Identifier: LGPL-2.1-or-later`.

## First contribution?

Good places to start:

- Improve the README: fix typos, add a minimal client/service example, document DSP
- Add an automated test setup (for example with CTest)
- Try the build on a platform or compiler that isn't covered yet and report the results

If you're not sure where to begin, open an issue and ask. No question is too small.

## License

By contributing, you agree that your contributions will be licensed under the project's license, **LGPL-2.1-or-later**. See [LICENSE](LICENSE).
