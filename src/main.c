#include "backend/backend.h"

#include "coderep/allocator/arena.h"
#include "coderep/cli/cli.h"
#include "coderep/data_array.h"
#include "coderep/data_type/bimap.h"
#include "coderep/macros.h"

#include <stdbool.h>
#include <unistd.h>

int main(int argc, const char **argv)
{
	struct arena *arena = arena_alloc(8 * 1024 * 1024);

	/* struct cli *cli = cli_alloc();

	cli_parse(cli, argv, argc);

	if (cli->lang._backend == NULL) PRINT_ERR("Code Report needs a backend. Chose one with '--lang'. You can see the current ones with '--help'");
	else {
		struct data_array *datas = backend_parse(cli->lang._backend, arena, cli->input);

		data_array_print(datas, &cli->data);

		data_array_drop(&datas);
	}

	cli_drop(&cli); */
	struct string *name = string_from_cstr(&arena->abstract, "Binary map");
	struct bimap *bimap = bimap_alloc(&arena->abstract, &name, 8);
	name = string_from_cstr(&arena->abstract, "Brute");
	bimap_push(bimap, &name);
	name = string_from_cstr(&arena->abstract, "2");
	bimap_push(bimap, &name);

	arena_drop(&arena);
}

