#ifndef CODEREP_BACKEND_H
#define CODEREP_BACKEND_H

#include "backend/lua/lua.h"

#include <stddef.h>

typedef struct data_array *(*parse_files_t)();

struct backend {
	const char *const name;
	const char *const identifier;
	parse_files_t parsing_finction;
};

static const struct backend BACKEND_REGISTER[] = {
	{ "Lua", "lua", (parse_files_t)lua_parse },
};

static const size_t BACKEND_REGISTER_L = sizeof(BACKEND_REGISTER) / sizeof(struct backend);

#endif
