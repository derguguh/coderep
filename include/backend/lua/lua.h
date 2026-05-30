#ifndef BACKEND_LUA_LUA_H
#define BACKEND_LUA_LUA_H

#include "backend/backend.h"

#include <stddef.h>

size_t lua_request();

void lua_init(struct backend_data *data);

void lua_parse_file(struct backend_data *data, struct arena *file_allocator, struct file *file);

void lua_deinit(struct backend_data *data);

#endif
