#include "backend/backend.h"

#include "backend/lua/lua.h"
#include "backend/text/text.h"

#include "coderep/allocator/arena.h"
#include "coderep/consts.h"
#include "coderep/data_array.h"
#include "coderep/tool/file.h"
#include "coderep/tool/file_buffer.h"

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

struct data_array *backend_parse(const struct backend *backend, struct arena *arena, struct input *input)
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

	size_t bytes_read = 0;
	size_t total_lines = 0;

	for (size_t i = 0; i < input->files_l; i++) {

		backend_print_reading_files(i + 1, input->files_l);

		arena_clear(parsing_arena);
		file_open(file, input->files[i]);
		if (file->buffer->_file == NULL) continue;

		backend->parsing_function(&data, parsing_arena, file);

		bytes_read += file->buffer->block * BUFFER_SIZE + file->buffer->buffer_length;
		total_lines += file->line;
	}
	putchar('\n');

	struct string *tmp_str = string_alloc(&arena->abstract, sizeof(size_t) * 8);
	snprintf(tmp_str->chars, sizeof(size_t) * 8, "%zu", bytes_read);

	struct string *tmp_name = string_from_cstr(&arena->abstract, "file.bytes_read");
	struct brute *tmp_brute = brute_alloc(&arena->abstract, &tmp_name, &tmp_str);

	data_array_push_brute(data_array, &tmp_brute);

	tmp_str = string_alloc(&arena->abstract, sizeof(size_t) * 8);
	snprintf(tmp_str->chars, sizeof(size_t) * 8, "%zu", total_lines);

	tmp_name = string_from_cstr(&arena->abstract, "file.lines");
	tmp_brute = brute_alloc(&arena->abstract, &tmp_name, &tmp_str);

	data_array_push_brute(data_array, &tmp_brute);

	if (backend->_deinit_function != NULL) backend->_deinit_function(&data);

	arena_drop(&parsing_arena);
	file_drop(&file);

	return data_array;
}

