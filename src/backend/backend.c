#include "backend/backend.h"

#include "backend/lua/lua.h"
#include "backend/text/text.h"
#include "backend/c/c.h"

#include "coderep/allocator/allocator.h"
#include "coderep/allocator/arena.h"
#include "coderep/consts.h"
#include "coderep/data_array.h"
#include "coderep/tool/file.h"
#include "coderep/tool/file_buffer.h"

#include <stdio.h>

#define PROGRESS_BAR_SIZE 40

const struct backend BACKEND_REGISTER[] = {
	{ "Text", "text", text_request, text_init, text_parse_file, text_deinit },
	{ "Lua", "lua", lua_request, lua_init, lua_parse_file, lua_deinit },
	{ "C", "c", c_request, c_init, c_parse_file, c_deinit },
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
	char bar_buffer[PROGRESS_BAR_SIZE + 1];

	float tmp = (float)current / total;
	size_t percent_complete = (size_t)(tmp * PROGRESS_BAR_SIZE);
	
	for (size_t i = 0; i < percent_complete; i++) bar_buffer[i] = '=';
	for (size_t i = percent_complete; i < PROGRESS_BAR_SIZE; i++) bar_buffer[i] = ' ';
	bar_buffer[PROGRESS_BAR_SIZE] = '\0';

	printf("\r" ANSI_GREEN "[Reading files ["
		"%s"
		"]" ANSI_WHITE " %lu/%lu"
		ANSI_GREEN "]" ANSI_RESET,
		bar_buffer, current, total);
	fflush(stdout);
}

void backend_push_file_data(struct arena *allocator, struct data_array *array, size_t bytes_read, size_t total_lines)
{
	struct string *tmp_name = string_from_cstr(&allocator->abstract, "file.bytes_read");
	struct brute *tmp_brute = brute_from_size_t(&allocator->abstract, &tmp_name, bytes_read);

	data_array_push_brute(array, &tmp_brute);

	tmp_name = string_from_cstr(&allocator->abstract, "file.lines");
	tmp_brute = brute_from_size_t(&allocator->abstract, &tmp_name, total_lines);

	data_array_push_brute(array, &tmp_brute);
}

struct data_array *backend_parse(const struct backend *backend, struct arena *arena, struct input *input)
{
	struct data_array *data_array = data_array_alloc();
	struct file *file = file_alloc(&arena->abstract);
	struct arena *parsing_arena = arena_alloc(1024 * 1024);
	struct allocator default_allocator = default_get_allocator();

	struct backend_data data = (struct backend_data) {
		.allocator = arena,
		.array = data_array,
		.context = backend_fulfill_request(backend, arena),
		.scratch_buffer = string_alloc(&default_allocator, 128),
	};

	if (backend->_init_function != NULL) backend->_init_function(&data);

	size_t bytes_read = 0;
	size_t total_lines = 0;

	for (size_t i = 0; i < input->files_l; i++) {

		backend_print_reading_files(i + 1, input->files_l);

		arena_clear(parsing_arena);
		string_clear(data.scratch_buffer);
		file_open(file, input->files[i]);
		if (file->buffer->_file == NULL) continue;

		backend->parsing_function(&data, parsing_arena, file);

		bytes_read += file->buffer->block * BUFFER_SIZE + file->buffer->buffer_length;
		total_lines += file->line;
	}
	putchar('\n');

	backend_push_file_data(arena, data_array, bytes_read, total_lines);

	if (backend->_deinit_function != NULL) backend->_deinit_function(&data);

	string_drop(&data.scratch_buffer);
	arena_drop(&parsing_arena);
	file_drop(&file);

	return data_array;
}

