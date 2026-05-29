#include "backend/lua/lua.h"

#include "backend/lua/lexer.h"
#include "backend/lua/lexical_table.h"
#include "backend/lua/token.h"
#include "coderep/cli/input.h"
#include "coderep/consts.h"
#include "coderep/data_array.h"
#include "coderep/tool/file.h"

struct data_array *lua_parse(const struct input *input)
{
	struct data_array *data_array = data_array_alloc();

	struct lua_lexical_table *lexical_table = lua_lexical_table_alloc();

	struct file *file = file_alloc();
	struct string *string = string_alloc();

	for (size_t i = 0; i < input->files_l; i++) {

		file_open(file, input->files[i]);
		if (file->buffer->_file == NULL) continue;
		printf(ANSI_ESC ANSI_GREEN "[File "
				ANSI_ESC ANSI_RESET "%lu/%lu"
				ANSI_ESC ANSI_GREEN ":"
				ANSI_ESC ANSI_RESET "%s" ANSI_ESC ANSI_GREEN
				ANSI_ESC ANSI_GREEN "]"
				ANSI_ESC ANSI_RESET " \n", i, input->files_l, input->files[i]);

		lua_lexer_skip_shebang(file);
		struct lua_token token = lua_lexer_next(file, lexical_table);
		while (token.type != TOKEN_TYPE_EOF) token = lua_lexer_next(file, lexical_table);
	}

	string_drop(&string);
	file_drop(&file);

	lua_lexical_table_append_to_data_array(lexical_table, data_array);

	lua_lexical_table_drop(&lexical_table);

	return data_array;
}

