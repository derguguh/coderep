#include "backend/parsing.h"

#include "coderep/allocator/c_allocator.h"
#include "coderep/cli/cli.h"
#include "coderep/cli/config.h"
#include "coderep/cli/help.h"
#include "coderep/macros.h"
#include "coderep/timer.h"

#include <time.h>
#include <stdbool.h>
#include <unistd.h>


int main(int argc, const char **argv)
{
	if (argc < 2) {
		help_print(NULL, NULL);
		return 0;
	}

	struct timespec begin;
	timer_start(&begin);

	struct c_allocator *c_allocator = c_allocator_alloc();
	struct cli *cli = cli_alloc(&c_allocator->abstract);

	const struct config *config = cli_parse_args(cli, argv, argc);

	if (config->procced) {
		if (config->_backend != NULL) parse_backend(config, c_allocator);
		else PRINT_ERR("Code Report needs a backend, use '--help' or '-h' to get a list of backends");
	}

	enum verbosity verbosity = config->verbosity;
	cli_drop(&cli);
	if (verbosity == VERBOSE) c_allocator_print(c_allocator);
	c_allocator_drop(&c_allocator);
	printf("Total time: %.3fs\n", timer_elapsed(&begin));
}

