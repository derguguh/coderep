#ifndef CODEREP_BACKEND_BACKEND_H
#define CODEREP_BACKEND_BACKEND_H

#include "backend/general/general.h"
#include "backend/lua/lua.h"

#include <stddef.h>

typedef struct data_array *(*parse_files_t)(const struct input *files);

struct backend {
	const char *const name;
	const char *const identifier;
	parse_files_t parsing_function;
};

static const struct backend BACKEND_REGISTER[] = {
	{ "Lua", "lua", lua_parse },
	{ "General text", "general", general_parse },
};

static const size_t BACKEND_REGISTER_L = sizeof(BACKEND_REGISTER) / sizeof(struct backend);

#endif
