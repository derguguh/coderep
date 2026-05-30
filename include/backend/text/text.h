#ifndef BACKEND_TEXT_TEXT_H
#define BACKEND_TEXT_TEXT_H

#include "backend/backend.h"

#include <stddef.h>

size_t text_request();

void text_init(struct backend_data *data);

void text_parse_file(struct backend_data *data, struct arena *ignore, struct file *file);

void text_deinit(struct backend_data *data);

#endif
