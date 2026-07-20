#include "backend/backend.h"

#include "backend/c/c.h"
#include "backend/keycount/alnum.h"
#include "backend/keycount/keycount.h"
#include "backend/lua/lua.h"
#include "backend/text/text.h"

const struct backend BACKEND_REGISTER[] = {
	{ "Text", "text", "Generic UTF-8 text",
		text_request, text_init, text_parse_file, text_deinit, false },
	{ "Lua", "lua", "Lua programming language",
		lua_request, lua_init, lua_parse_file, lua_deinit, false },
	{ "Key counter", "keycount", "Single and digraphs counting",
		keycount_request, keycount_init, keycount_parse_file, keycount_deinit, false },
	{ "alpha numeric", "keycount.alnum", "Single and digraphs counting on alpha numeric",
		keycount_request, keycount_init, keycount_alnum_parse_file, keycount_deinit, false },
	{ "C", "c", "Default C version (WIP)",
		c_request, c_init, c_parse_file, c_deinit, false },
	{ "C23", "c.23", "C 2024 version (WIP)",
		c_request, c_init, c_parse_file, c_deinit, false },
	{ "C99", "c.99", "C 1999 version (WIP)",
		c_request, c_init, c_parse_file, c_deinit, false },
};

const size_t BACKEND_REGISTER_L = sizeof(BACKEND_REGISTER) / sizeof(struct backend);

