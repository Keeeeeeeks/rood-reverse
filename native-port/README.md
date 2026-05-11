# Native Port Work Area

This directory is intentionally separate from the PS1 matching build. Files here
describe host-runtime boundaries for a future native port, but they are not
included by `src/`, `config/`, or the root `Makefile`.

Current scope:

- document hardware-facing seams that native code must replace;
- define small header-only contracts for those seams;
- avoid implementation and build-system coupling until the matching decompilation
  oracle is farther along.

Do not include these headers from PS1 reconstruction sources. When a native build
target exists later, it should live under this directory or another isolated host
build tree.
