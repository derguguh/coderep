#include "backend/backend.h"

#include "backend/lua/lua.h"
#include "backend/text/text.h"
#include "backend/c/c.h"
#include "backend/keycount/keycount.h"
#include "backend/keycount/alnum.h"

#include "coderep/allocator/allocator.h"
#include "coderep/allocator/arena.h"
#include "coderep/consts.h"
#include "coderep/report.h"
#include "coderep/timer.h"
#include "coderep/tool/file.h"
#include "coderep/tool/file_buffer.h"

#include <stdio.h>
#include <time.h>

#define PROGRESS_BAR_SIZE 40

const struct backend BACKEND_REGISTER[] = {
	{ "Text", "text", "General UTF8 text",
		text_request, text_init, text_parse_file, text_deinit, false },
	{ "Lua", "lua", "Lua programming language",
		lua_request, lua_init, lua_parse_file, lua_deinit, false },
	{ "C", "c", "ISO C programming language version",
		c_request, c_init, c_parse_file, c_deinit, false },
	{ "Key counter", "keycount", "Single and digraphs counting",
		keycount_request, keycount_init, keycount_parse_file, keycount_deinit, false},
	{ "Key counter (alpha numeric only)", "keycount.alnum", "Single and digraphs counting on alpha numeric",
		keycount_request, keycount_init, keycount_alnum_parse_file, keycount_deinit, false},
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

struct report *backend_parse(const struct backend *backend, struct allocator *default_allocator, struct arena *arena, struct input *input)
{
	struct arena *parsing_arena = arena_alloc(1024 * 1024);
	struct file *file = file_alloc(&arena->abstract);
	struct report *report = report_alloc(default_allocator);

	struct backend_context context = (struct backend_context) {
		.allocator = arena,
		.report = report,
		.requested = backend_fulfill_request(backend, arena),
		.scratch_buffer = string_alloc(default_allocator, 128),
		.profile = (struct bucket_profile) {
			.minimal = 1024,
			.light = 2 * 1024,
			.medium = 8 * 1024,
			.heavy = 32 * 1024,
			.extreme = 128 * 1024,
		}
	};
	struct file_report file_report = (struct file_report) {
		.total_reads = input->files_l,
	};
	struct timespec timer;

	timer_start(&timer);
	if (backend->_init_function != NULL) backend->_init_function(&context);
	statistics_double_push(&file_report.total_init_time, timer_elapsed(&timer));

	for (size_t i = 0; i < input->files_l; i++) {
		backend_print_reading_files(i + 1, input->files_l);

		arena_clear(parsing_arena);
		string_clear(context.scratch_buffer);
		file_open(file, input->files[i]);
		if (file->buffer->_file == NULL) {
			file_report.failed_reads++;
			continue;
		}
		timer_start(&timer);
		backend->parsing_function(&context, parsing_arena, file);
		statistics_double_push(&file_report.total_file_parse_time, timer_elapsed(&timer));

		statistics_size_t_push(&file_report.bytes_read, file->buffer->block * BUFFER_SIZE + file->buffer->buffer_length);
		statistics_size_t_push(&file_report.total_line, file->line);
	}
	putchar('\n');

	timer_start(&timer);
	if (backend->_deinit_function != NULL) backend->_deinit_function(&context);
	statistics_double_push(&file_report.total_deinit_time, timer_elapsed(&timer));

	report_push_file_report(report, &file_report);
	string_drop(&context.scratch_buffer);
	arena_drop(&parsing_arena);
	file_drop(&file);

	return report;
}

