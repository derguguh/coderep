#include "backend/parsing.h"

#include "backend/backend.h"
#include "coderep/allocator/arena.h"
#include "coderep/cli/config.h"
#include "coderep/consts.h"
#include "coderep/report.h"
#include "coderep/timer.h"
#include "coderep/tool/file.h"

#include <stdio.h>
#include <time.h>

#define PROGRESS_BAR_SIZE 40

// Register moved to backend_register.c

void *backend_fulfill_request(const struct backend *backend, struct arena *allocator)
{
	size_t size_to_alloc;
	if (backend->_request != NULL) size_to_alloc = backend->_request();
	else size_to_alloc = 0;

	void *ptr = NULL;
	if (size_to_alloc > 0) ptr = arena_allocate(allocator, size_to_alloc);

	return ptr;
}

void backend_print_reading_files(size_t current, size_t total, bool do_color, bool in_new_line)
{
	char bar_buffer[PROGRESS_BAR_SIZE];

	float tmp = (float)current / total;
	size_t percent_complete = (size_t)(tmp * PROGRESS_BAR_SIZE);
	
	for (size_t i = 0; i < percent_complete; i++) bar_buffer[i] = '=';
	for (size_t i = percent_complete; i < PROGRESS_BAR_SIZE; i++) bar_buffer[i] = ' ';

	if (!in_new_line) printf("\r");
	if (do_color) printf(ANSI_GREEN);
	printf("[Reading files ["
		"%*s"
		"]", PROGRESS_BAR_SIZE, bar_buffer);
	if (do_color) printf(ANSI_WHITE);
	printf(" %lu/%lu", current, total);
	if (do_color) printf(ANSI_GREEN);
	printf("]");
	if (do_color) printf(ANSI_RESET);
	if (in_new_line) printf("\n");
	else fflush(stdout);
}

void parse_file(const struct config *config, struct backend_context *context, struct arena *temporary_arena)
{
	struct file *file = file_alloc(&context->allocator->abstract);

	size_t paths_l = config->paths_l;
	struct timespec timer;
	timer_start(&timer);

	for (size_t i = 0; i < paths_l; i++) {
		if (config->verbosity != QUIET) backend_print_reading_files(i + 1, paths_l, config->do_color, config->verbosity == VERBOSE);

		arena_clear(temporary_arena);
		string_clear(context->scratch_buffer);
		file_open(file, string_to_cstr(config->paths[i]));
		if (file->buffer->_file == NULL) {
			context->file_report.failed_reads++;
			continue;
		}
		timer_start(&timer);
		config->_backend->parsing(context, temporary_arena, file);
		statistics_double_push(&context->file_report.total_file_parse_time, timer_elapsed(&timer));

		statistics_size_t_push(&context->file_report.bytes_read, file->buffer->block * BUFFER_SIZE + file->buffer->buffer_length);
		statistics_size_t_push(&context->file_report.total_line, file->line);
	}
	putchar('\n');

	file_drop(&file);
}

void parse_backend(const struct config *config, struct c_allocator *allocator)
{
	if (config->_backend == NULL) return;
	struct report *report = report_alloc(&allocator->abstract);
	struct arena *persistent_arena = arena_alloc(allocator, 8 * 1024 * 1024);
	struct arena *temporary_arena = arena_alloc(allocator, 1024 * 1024);

	struct backend_context context = (struct backend_context) {
		.allocator = persistent_arena,
		.report = report,
		.file_report = {
			.total_reads = config->paths_c,
		},
		.requested = backend_fulfill_request(config->_backend, persistent_arena),
		.scratch_buffer = string_alloc(&allocator->abstract, 128),
		.profile = {
			.minimal = config->bucket_baseline,
			.light = config->bucket_baseline * 2,
			.medium = config->bucket_baseline * 4,
			.heavy = config->bucket_baseline * 8,
			.extreme = config->bucket_baseline * 16,
		}
	};
	struct timespec timer;

	timer_start(&timer);
	if (config->_backend->_init != NULL) config->_backend->_init(&context);
	statistics_double_push(&context.file_report.total_init_time, timer_elapsed(&timer));

	if (config->_backend->binary) {
		printf("Binary backends aren't done yet");
	} else parse_file(config, &context, temporary_arena);

	timer_start(&timer);
	if (config->_backend->_deinit != NULL) config->_backend->_deinit(&context);
	statistics_double_push(&context.file_report.total_deinit_time, timer_elapsed(&timer));

	report_push_file_report(report, &context.file_report, config->show_time);

	report_print(report, &config->data, config->do_color);

	string_drop(&context.scratch_buffer);
	report_drop(&report);
	arena_drop(&temporary_arena);
	arena_drop(&persistent_arena);
}

