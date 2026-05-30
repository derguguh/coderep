#ifndef CODEREP_BACKEND_BACKEND_H
#define CODEREP_BACKEND_BACKEND_H

#include "coderep/cli/data.h"
#include "coderep/cli/input.h"
#include "coderep/tool/file.h"

#include <stddef.h>

struct backend_data {
	struct arena *allocator;
	struct data_array *array;
	void *context;
};

typedef size_t (*request_t)();
typedef void (*init_t)(struct backend_data *data);
typedef void (*parse_file_t)(struct backend_data *data, struct arena *temp_allocator, struct file *file);
typedef void (*deinit_t)(struct backend_data *data);

struct backend {
	const char *const name;
	const char *const identifier;
	request_t _request_function;
	init_t _init_function;
	parse_file_t parsing_function;
	deinit_t _deinit_function;
};

extern const struct backend BACKEND_REGISTER[];

extern const size_t BACKEND_REGISTER_L;

struct data_array *backend_parse(const struct backend *backend, struct arena *allocator, struct input *input, struct data output_modes);

#endif
