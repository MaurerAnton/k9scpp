# k9scpp — Kubernetes Cluster Management TUI (C++ port of k9s)

A zero-dependency C++ port of [k9s](https://github.com/derailed/k9s) — a terminal UI for managing Kubernetes clusters with keyboard-driven navigation.

## Why k9scpp?

The original [k9s](https://github.com/derailed/k9s) requires the Go toolchain plus dozens of modules. k9scpp compiles with a single `make` using only C++17 and standard Linux headers.

## Quick Start

```bash
make
./k9scpp
```

## Features

- Browse pods, deployments, services, nodes, and more
- Keyboard-driven navigation
- Real-time resource monitoring
- Log viewing and filtering
- Port forwarding
- YAML resource editing
- Multi-namespace support

## Build

```bash
make
```
Requires: GCC 10+ or Clang 12+, GNU Make, libcurl, nlohmann/json (vendored)
