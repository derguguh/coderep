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

## Roadmap

The following features are planned and not yet implemented: 
 - `--filter` flag for scoping output to specific token categories
 - Lua bindings for expanding Code Report without needing to use C
 - extended Lua backend with syntax-level data extraction
 - additional language backends including C

## License

Code Report is licensed under the GNU Lesser General Public License v2.1 or later. See LICENSE for details.
