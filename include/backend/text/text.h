#ifndef BACKEND_TEXT_TEXT_H
#define BACKEND_TEXT_TEXT_H

#include "backend/backend.h"

#include <stddef.h>

size_t text_request();

void text_init(struct backend_context *data);

void text_parse_file(struct backend_context *data, struct arena *ignore, struct file *file);

void text_deinit(struct backend_context *data);

#endif
