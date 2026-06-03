# k9scpp — Kubernetes CLI Explorer (C++ port of k9s)

A zero-dependency C++ wrapper around `kubectl` for listing Kubernetes resources with colored output.

## Why k9scpp?

The original [k9s](https://github.com/derailed/k9s) requires Go plus dozens of modules. k9scpp compiles with a single `make` using only C++17.

## Quick Start

```bash
make
./k9scpp                    # List pods in default namespace
./k9scpp -n kube-system     # List pods in kube-system
./k9scpp -A                 # Pods across all namespaces
./k9scpp --svc              # List services
./k9scpp --deploy           # List deployments
./k9scpp --ns               # List namespaces
```

## Features

- Color-coded pod listing (green=running, yellow=pending, red=other)
- Pods, services, deployments, and namespaces
- Namespace selection (`-n`) or all namespaces (`-A`)
- Zero external dependencies — shells out to `kubectl`

## Note

This is **not** an interactive TUI — it's a static CLI formatter for `kubectl get` output. For interactive cluster browsing, use the original [k9s](https://github.com/derailed/k9s).

## Build

```bash
make
```
Requires: GCC 10+ or Clang 12+, GNU Make, `kubectl` in PATH
