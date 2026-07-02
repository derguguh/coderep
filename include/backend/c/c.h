#ifndef BACKEND_C_C_H
#define BACKEND_C_C_H

#include "backend/backend.h"

#include <stddef.h>

size_t c_request();

void c_init(struct backend_context *data);

void c_parse_file(struct backend_context *data, struct arena *file_allocator, struct file *file);

void c_deinit(struct backend_context *data);

#endif
