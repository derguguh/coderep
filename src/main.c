#include "coderep/tools/file_buffer.h"

#include <stdbool.h>
// #include "coderep/tools/cli/data.h"

int main(int argc, const char **argv)
{
	// struct data data = data_create();
	//
	// data_parse(&data, argv, argc, 1);

	struct string *file_path = string_from_cstr("/home/derguguh/projects/coderep/tests/lua/lexer/keywords.lua");
	struct file_buffer *lua_file_buffer = file_buffer_alloc(file_path);

	file_buffer_print(lua_file_buffer, false);
	file_buffer_print(lua_file_buffer, true);

	for (int i = 0; i < 19; i++) file_buffer_advance(lua_file_buffer);

	file_buffer_drop(&lua_file_buffer);
	string_drop(&file_path);
}

