#ifndef CODEREP_CLI_CLI_H
#define CODEREP_CLI_CLI_H

#include "coderep/cli/lang.h"
#include "coderep/cli/input.h"
#include "coderep/cli/source.h"
#include "coderep/cli/filter.h"
#include "coderep/cli/data.h"

#include <stddef.h>

enum flag_locations {
	FLAG_LOCATIONS_LANG,
	FLAG_LOCATIONS_INPUT,
	FLAG_LOCATIONS_SOURCE,
	FLAG_LOCATIONS_FILTER,
	FLAG_LOCATIONS_DATA,
};

typedef int (*parse_function_t) (void *self, const char **const argv, int argc, int index);

struct flags_table {
	void *structure;
	const char *const flag_name;
	parse_function_t parsing;
};

struct cli {
	struct flags_table *table;
	struct lang lang;
	struct input *input;
	struct source *source;
	struct filter filter;
	struct data data;
};

static const struct flags_table TABLE[] = {
	{ NULL, "--lang", (parse_function_t)lang_parse },
	{ NULL, "--input", (parse_function_t)input_parse },
	{ NULL, "--source", (parse_function_t)source_parse },
	{ NULL, "--filter", (parse_function_t)filter_parse },
	{ NULL, "--data", (parse_function_t)data_parse },
};

static const size_t TABLE_L = sizeof(TABLE) / sizeof(struct flags_table);

struct cli *cli_alloc();

void cli_parse(struct cli *self, const char **const argv, int argc);

void cli_drop(struct cli **self);

#endif
