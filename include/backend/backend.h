#ifndef CODEREP_BACKEND_BACKEND_H
#define CODEREP_BACKEND_BACKEND_H

#include "coderep/cli/input.h"
#include "coderep/tool/file.h"

#include <stddef.h>

struct bucket_profile {
	size_t extreme;
	size_t heavy;
	size_t medium;
	size_t light;
	size_t baseline;
};

struct backend_context {
	struct arena *allocator;
	struct report *report;
	void *requested;
	// Common utilities
	struct string *scratch_buffer;
	struct bucket_profile profile;
};

typedef size_t (*request_t)();
typedef void (*init_t)(struct backend_context *data);
typedef void (*parse_file_t)(struct backend_context *data, struct arena *temp_allocator, struct file *file);
typedef void (*deinit_t)(struct backend_context *data);

struct backend {
	const char *const name;
	const char *const identifier;
	const char *const description;
	request_t _request_function;
	init_t _init_function;
	parse_file_t parsing_function;
	deinit_t _deinit_function;
	bool binary;
};

struct file_report {
	// General
	size_t bytes_read;
	size_t total_reads;
	size_t failed_reads;
	bool binary;
	// Binary (struct file_buffer) only
	size_t total_line;
	// Standard (struct file) only
};

extern const struct backend BACKEND_REGISTER[];

extern const size_t BACKEND_REGISTER_L;

struct report *backend_parse(const struct backend *backend, struct arena *allocator, struct input *input);

#endif
