# Code Report

Code Report (coderep) is a static data extraction tool. It reads source files and collects frequency data about their lexical and syntactic structure, producing a complete report of every token, keyword, identifier, and symbol found. Think of it as perf for source code — not a linter, not a compiler, just a precise data collector.
It supports multiple language backends. Any file format that can be read and tokenized can have a backend written for it.

See [Backends](#backends) to more info about current backends

## Building

### Dependencies
GNU Makefile, your favorite C compiler (default as Clang)

```bash
make release
```

## Usage
```bash
coderep --lang lua --input $(find . -name "*.lua")
coderep --lang text --input $(find . -name "*.c")
```

> Use `coderep --help` for more info

## Backends

  - `text` processes any UTF-8 readable file as plain text, collecting character, word, and number frequencies. It serves as the fallback backend for formats without a dedicated implementation
  - `lua` provides a complete Lua lexer, recognizing all keywords, operators, string literals, numeric literals, and identifiers according to the Lua 5.4 specification, including long strings and long comments with arbitrary bracket levels. It currently don't have syntax data extraction

## Performance

Code Report is designed to handle large codebases efficiently. It uses a custom arena allocator and string interning to minimize heap allocation overhead. On a mid-range desktop, it processes the entire Linux kernel source tree in under a minute.

## Contributing

Code Report follows a strict coding style. Contributions are expected to match it before being accepted.

The codebase uses Linux Kernel coding style as its base. This covers naming conventions, brace placement, indentation, and general formatting. Familiarize yourself with it before contributing.
Beyond that, the following conventions are specific to this project and must be respected:
  - Structs are either heap-only or free to use in both heap and stack, and this is determined at design time, not at the call site. A heap-only struct is identified by the presence of an _alloc function. If a struct has _alloc, it must have a corresponding _drop, and may have a _clone. A struct without _alloc has no ownership rules enforced by the API and may be used freely.
  - `*_alloc` functions allocate and perform basic initialization. They exit the program on allocation failure rather than returning NULL, so callers do not need to check the return value. `*_drop` functions perform all freeing work and nullify the pointer they receive. After calling `*_drop`, the original variable is NULL. `*_clone` functions always perform a deep copy.
  - Double pointer parameters signal ownership transfer. When a function accepts `struct foo **`, it takes ownership of the pointed-to value. The caller should not use the original variable after the call, as the function may nullify it. `frequency_push` is an example of this pattern.

All allocation beyond CLI goes through the allocator interface. Backends and data types do not call malloc or free directly.

The main branch always reflects the most recent release. Active development happens on dev. Feature and fix branches must be prefixed with dev-.

## Roadmap

The following features are planned and not yet implemented: 
  - `--filter` flag for scoping output to specific token categories
  - Lua bindings for expanding Code Report without needing to use C
  - Extended Lua backend with syntax-level data extraction
  - Additional language backends including C
  - More tools to make writing backends easier

## License

[GPL2-or-later](LICENSE)
