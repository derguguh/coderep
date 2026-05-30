# Writing a Backend

A backend is responsible for analyzing files and storing the extracted data.

To create a backend, implement the callbacks below and register the backend in `BACKEND_REGISTER`.

## Backend Lifecycle

The execution flow is:

```text
request_t
    ↓
init_t
    ↓
parse_file_t (once per file)
    ↓
deinit_t
```

### request_t

```c
typedef size_t (*request_t)();
```

Called before any file is analyzed.

The returned value is used to allocate a persistent memory region for the backend. This memory is available through `struct backend_data`, via `void *context`.

Most backends return the size of a context or lexer structure:

```c
size_t text_request()
{
	return sizeof(struct text_lexer);
}
```

The maximum supported allocation size is approximately 8 MiB.

---

### init_t

```c
typedef void (*init_t)(struct backend_data *data);
```

Called once before any file is analyzed.

Use this function to initialize the memory requested by `request_t`.

> *See `"backend/text/text.h"` for a header and `"backend/text/text.c"` for a source code*

---

### parse_file_t

```c
typedef void (*parse_file_t)(struct backend_data *data, struct arena *temp_allocator, struct file *file);
```

Called once for every input file.

This is where the analysis happens.

`temp_allocator` is automatically cleaned after the file is processed and should be used for temporary allocations such as:

  - Tokens
  - AST nodes
  - Scratch buffers
  - Parser state

Do not manually free memory allocated from this arena.

> *See `"backend/text/lexer.h"` for a header and `"backend/text/lexer.c"` for a source code*

---

### deinit_t

```c
typedef void (*deinit_t)(struct backend_data *data);
```

Called after all files have been analyzed.

This function can be used for:
  - Post-file work (like semantic analysis)
  - Pushing data into `struct data_array *array`
  - Freeing buffers allocated outside `struct arena *allocator`

Any data stored in `backend_data` remains available.

---

## Registering a Backend

Backends are registered through `BACKEND_REGISTER`.

Example:

```c
const struct backend BACKEND_REGISTER[] = {
	{ "Text", "text", text_request, text_init, text_parse_file, text_deinit },
    /* other backends */
};
```

The short name is used by the command line:

```bash
coderep --lang text --input $(find --name "*.txt")
```

## Data Output

Backends store information in the data types provided by Coderep.

The framework handles output formatting automatically.

Data types currently available:
  - `frequency`
  - `brute`

A backend only needs to collect data and store it on `struct data_array *array` located in `struct backend_data`. The framework is responsible for printing the results.

## Memory Management

Coderep provides memory management utilities so backend authors rarely need manual allocation.

Typical usage:
  - Persistent backend state → memory returned by `request_t` or allocated via `struct arena *allocator` located inside `struct backend_data`
  - Per-file temporary data → `struct arena *temp_allocator`
  - Temporary token construction → reusable `struct string *scratch_buffer`

In most cases, backend code should not need explicit `free()` or `default_deallocate(NULL, void *ptr)` calls.

## Lexer Helpers

Common lexer utilities are available in `"backend/general/lexer.h"`

Including helpers for:

  - Whitespace skipping
  - Integer parsing
  - Hex numbers
  - Scientific suffix
  - *More are in development*

