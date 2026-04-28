#include "coderep/tools/cli/data.h"
#include "coderep/consts.h"

#include <stdio.h>
#include <string.h>

struct flag_opt {
	char *cstr;
	enum flags flag;
};

const struct flag_opt flag_opts[] = {
	{ "none",		NONE },
	{ "raw",		RAW },
	{ "min",		MIN },
	{ "avg",		AVG },
	{ "max",		MAX },
	{ "moda",		MODA },
	{ "median",		MEDIAN },
	{ "value",		VALUE },
	{ "frequency",		FREQUENCY },
	{ "count",		COUNT },
	// Mixs
	{ "distribution",	DISTRIBUTION},
	{ "all",		ALL},
};
const size_t flag_opts_l = sizeof(flag_opts) / sizeof(struct flag_opt);

struct data data_create()
{
	struct data data = {
		DEFAULT_DISTRIBUTIVE,
		DEFAULT_BRUTE,
		DEFAULT_LIST,
	};

	return data;
}

enum flags parse_flag(char *value)
{
	for (int i = 0; i < flag_opts_l; i++) {
		if (strcasecmp(value, flag_opts[i].cstr) == 0) {

			return flag_opts[i].flag;
		}
	}

	fprintf(stderr, ANSI_ERR "'%s' is not a valid '--data' flag\n", value);
	return NONE;
}

int data_parse(struct data *self, char **argv, int argc, int index)
{
	if (index + 1 > argc) {

		fprintf(stderr, ANSI_ERR "'--data' option expects at least one flag\n");
		return index;
	}

	while (index < argc) {

		enum flags flag = parse_flag(argv[index]);
		printf("Flag n-%d: %s\n", index, argv[index]);
		printf("  Value got: %x\n", flag);
		index++;
	}

	return index - 1;
}

