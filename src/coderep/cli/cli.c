#include "coderep/cli/cli.h"

#include "coderep/cli/filter.h"
#include "coderep/macros.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cli *cli_alloc()
{
	struct cli *cli = malloc(sizeof(struct cli));
	struct flags_table *table = malloc(sizeof(struct flags_table) * TABLE_L);

	if (!cli || !table) goto defer;

	cli->lang = lang_create();
	cli->input = input_alloc();
	cli->source = source_alloc();
	cli->filter = filter_create();
	cli->data = data_create();

	cli->table = table;
	memcpy(cli->table, TABLE, sizeof(TABLE));

	cli->table[FLAG_LOCATIONS_HELP].structure = NULL;
	cli->table[FLAG_LOCATIONS_LANG].structure = &cli->lang;
	cli->table[FLAG_LOCATIONS_INPUT].structure = cli->input;
	cli->table[FLAG_LOCATIONS_SOURCE].structure = cli->source;
	cli->table[FLAG_LOCATIONS_FILTER].structure = &cli->filter;
	cli->table[FLAG_LOCATIONS_DATA].structure = &cli->data;

	return cli;

defer:
	free(cli);
	free(table);

	MALLOC_ERR_EXIT("struct cli and struct flags_table");
}

void cli_parse(struct cli *self, const char **argv, int argc)
{
	if (argc <= 1) {

		PRINT_ERR("Code Report needs an argument. You can use the '--help' flag for help");
		return;
	}

	int index = 1;
	while (index < argc) {
		bool detected = false;

		for (size_t i = 0; i < TABLE_L; i++) {
			if (strcmp(argv[index], self->table[i].flag_name) == 0) {
				index = self->table[i].parsing(self->table[i].structure, argv,  argc, index + 1);
				detected = true;
				break;
			}
		}

		if (!detected) {
			PRINTF_ERR("The option '%s' is not handled by the CLI", argv[index]);
			index++;
		}
	}
}

void cli_drop(struct cli **self)
{
	if (!self || !(*self)) return;

	free((*self)->table);
	(*self)->table = NULL;

	input_drop(&(*self)->input);
	source_drop(&(*self)->source);

	free(*self);
	*self = NULL;
}

