#include "backend/backend.h"

#include "backend/lua/lua.h"
#include "backend/text/text.h"

#include "coderep/allocator/arena.h"
#include "coderep/consts.h"
#include "coderep/data_array.h"
#include "coderep/tool/file.h"

#include <stdio.h>

const struct backend BACKEND_REGISTER[] = {
	{ "Text", "text", text_request, text_init, text_parse_file, text_deinit },
	{ "Lua", "lua", lua_request, lua_init, lua_parse_file, lua_deinit },
};

const size_t BACKEND_REGISTER_L = sizeof(BACKEND_REGISTER) / sizeof(struct backend);

void *backend_fulfill_request(const struct backend *backend, struct arena *allocator)
{
	size_t size_to_alloc;
	if (backend->_request_function != NULL) size_to_alloc = backend->_request_function();
	else size_to_alloc = 0;

	void *ptr = NULL;
	if (size_to_alloc > 0) ptr = arena_allocate(allocator, size_to_alloc);

	return ptr;
}

void backend_print_reading_files(size_t current, size_t total)
{
	const size_t progress_bar = 40;
	printf("\r" ANSI_GREEN "[Reading files [");

	float tmp = (float)current / total;
	size_t percent_complete = (size_t)(tmp * progress_bar);
	
	for (size_t i = 0; i < percent_complete; i++) putchar('=');
	for (size_t i = percent_complete; i < progress_bar; i++) putchar(' ');

	printf( "]" ANSI_WHITE " %lu/%lu"
		ANSI_GREEN "]" ANSI_RESET,
		current, total);

	fflush(stdout);
}

struct data_array *backend_parse(const struct backend *backend, struct arena *arena, struct input *input, struct data output_modes)
{
	struct data_array *data_array = data_array_alloc();
	struct file *file = file_alloc(&arena->abstract);
	struct arena *parsing_arena = arena_alloc(1024 * 1024);

	struct backend_data data = (struct backend_data) {
		.allocator = arena,
		.array = data_array,
		.context = backend_fulfill_request(backend, arena),
	};

	if (backend->_init_function != NULL) backend->_init_function(&data);

	for (size_t i = 0; i < input->files_l; i++) {

		backend_print_reading_files(i + 1, input->files_l);

		arena_clear(parsing_arena);
		file_open(file, input->files[i]);
		if (file->buffer->_file == NULL) continue;

		backend->parsing_function(&data, parsing_arena, file);
	}
	putchar('\n');

	if (backend->_deinit_function != NULL) backend->_deinit_function(&data);

	arena_drop(&parsing_arena);
	file_drop(&file);

	return data_array;
}

