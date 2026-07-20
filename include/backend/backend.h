#ifndef CODEREP_BACKEND_BACKEND_H
#define CODEREP_BACKEND_BACKEND_H

#include "coderep/cli/input.h"
#include "coderep/data_type/string.h"
#include "coderep/tool/file.h"

#include <stddef.h>

struct bucket_profile {
	size_t extreme;
	size_t heavy;
	size_t medium;
	size_t light;
	size_t minimal;
};

struct backend_context {
	struct arena *allocator;
	struct report *report;
	void *requested;
	// Common utilities
	struct string *scratch_buffer;
	struct bucket_profile profile;
};

typedef size_t (*request_fn)();
typedef void (*init_fn)(struct backend_context *context);
typedef void (*parse_file_fn)(struct backend_context *context, struct arena *temp_allocator, struct file *file);
typedef void (*deinit_fn)(struct backend_context *context);

struct backend {
	const char *const name;
	const char *const identifier;
	const char *const description;
	request_fn _request;
	init_fn _init;
	parse_file_fn parsing;
	deinit_fn _deinit;
	bool binary;
};

extern const struct backend BACKEND_REGISTER[];

extern const size_t BACKEND_REGISTER_L;

struct report *backend_parse(const struct backend *backend, struct allocator *default_allocator, struct arena *arena, struct input *input);

#endif
