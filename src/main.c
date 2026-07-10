#include "backend/backend.h"

#include "coderep/allocator/allocator.h"
#include "coderep/allocator/arena.h"
#include "coderep/allocator/block.h"
#include "coderep/cli/cli.h"
#include "coderep/report.h"
#include "coderep/macros.h"
#include "coderep/timer.h"

#include <time.h>
#include <stdbool.h>
#include <unistd.h>


int main(int argc, const char **argv)
{
	struct timespec begin;
	timer_start(&begin);
	struct arena *arena = arena_alloc(DEFAULT_BLOCK_SIZE);

	struct cli *cli = cli_alloc();

	cli_parse(cli, argv, argc);

	if (cli->lang._backend == NULL) PRINT_ERR("Code Report needs a backend. Chose one with '--lang'. You can see the current ones with '--help'");
	else {
		struct allocator default_allocator = default_get_allocator();
		struct report *report = backend_parse(cli->lang._backend, &default_allocator, arena, cli->input);

		report_print(report, &cli->data);

		report_drop(&report);
	}

	cli_drop(&cli);
	arena_drop(&arena);
	printf("Total time: %.3fs\n", timer_elapsed(&begin));
}

