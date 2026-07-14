# mem-manager

A memory management library for C++ and SystemVerilog testbenches, providing a
sparse memory model and an allocator used to back simulated address spaces.

## Overview

`mem-manager` provides two main components:

- **sparse_mem** — a sparse memory model that stores only written regions, so
  large address spaces can be represented without allocating backing storage for
  the whole range.
- **mem_manager** — an allocator/manager layered on top of the sparse memory,
  exposing a simple API to read, write, check, and allocate memory. It is usable
  from both C++ and SystemVerilog (via DPI) testbenches.

### API

- `read`
- `write`
- `check`
- `alloc`

## Getting Started

`mem-manager` builds with [Bazel](https://bazel.build/) (bzlmod).

```sh
# Build everything
bazel build //...

# Run the tests (including the Verilated SystemVerilog testbench)
bazel test //test:all
```

## Contributing

Contributions are welcome! Bug reports and feature requests are handled via
[GitHub Issues](https://github.com/tenstorrent/mem-manager/issues), and changes
are submitted via pull requests (reviewed weekly). See
[CONTRIBUTING.md](CONTRIBUTING.md) for build/test instructions and contribution
standards, and note that this project follows the
[Contributor Covenant Code of Conduct](CODE_OF_CONDUCT.md). To report a security
vulnerability, follow the process in [SECURITY.md](SECURITY.md).

## License

- [LICENSE](LICENSE) (Apache-2.0) — Overall license for this project, except where specified.
- [LICENSE-DOCS](LICENSE-DOCS) (CC-BY-4.0) — License for all documentation and images only.
- [LICENSE_understanding.txt](LICENSE_understanding.txt) — Tenstorrent's clarification of how the Apache-2.0 license applies to this repository.

This repository is [REUSE](https://reuse.software) compliant; per-file license
and copyright information is provided via inline SPDX headers and
[REUSE.toml](REUSE.toml). Third-party components retain their own licenses as
recorded in [NOTICE](NOTICE).
