#include "coderep/allocator/arena.h"
#include "coderep/cli/cli.h"
#include "coderep/data_array.h"
#include "coderep/macros.h"

#include <stdbool.h>

void execute_cli(const char **argv, int argc)
{
	struct cli *cli = cli_alloc();

	cli_parse(cli, argv, argc);

	if (cli->lang._backend == NULL) PRINT_ERR("Code Report needs a backend. Chose one with '--lang'. You can see the current ones with '--help'");
	else {
		struct data_array *datas = cli->lang._backend->parsing_function(cli->input);

		data_array_print(datas, cli->data);

		data_array_drop(&datas);
	}

	cli_drop(&cli);
}

int main(int argc, const char **argv)
{
	struct arena *arena = arena_alloc(DEFAULT_BLOCK_SIZE);

	arena_drop(&arena);
}

