#include "backend/backend.h"

#include "coderep/allocator/arena.h"
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
	struct arena *arena = arena_alloc(8 * 1024 * 1024);

	struct cli *cli = cli_alloc();

	cli_parse(cli, argv, argc);

	if (cli->lang._backend == NULL) PRINT_ERR("Code Report needs a backend. Chose one with '--lang'. You can see the current ones with '--help'");
	else {
		struct report *report = backend_parse(cli->lang._backend, arena, cli->input);

		report_print(report, &cli->data);

		report_drop(&report);
	}

	cli_drop(&cli);
	arena_drop(&arena);
	printf("Total time: %.3fs\n", timer_elapsed(&begin));
}

