#include "backend/lua/lua.h"

#include "backend/lua/lexer.h"
#include "backend/lua/lexical_table.h"
#include "backend/lua/token.h"
#include "coderep/cli/input.h"
#include "coderep/data_array.h"
#include "coderep/tool/file.h"

struct data_array *lua_parse(const struct input *input)
{
	struct data_array *data_array = data_array_alloc();

	struct lua_lexical_table *lexical_table = lua_lexical_table_alloc();

	struct file *file = file_alloc();

	for (int i = 0; i < input->files_l; i++) {

		file_open(file, input->files[i]);

		struct lua_token token = lua_lexer_next(file, lexical_table);
		while (token.type != TOKEN_TYPE_EOF) token = lua_lexer_next(file, lexical_table);
	}

	file_drop(&file);

	lua_lexical_table_append_to_data_array(lexical_table, data_array);

	lua_lexical_table_drop(&lexical_table);

	return data_array;
}

