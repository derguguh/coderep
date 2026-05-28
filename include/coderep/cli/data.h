#ifndef CODEREP_DATA_H
#define CODEREP_DATA_H

#include "coderep/data_type/output_mode.h"

#include <stddef.h>

struct data {
	enum output_mode frequency;
	enum output_mode brute;
	enum output_mode list;
};

struct flag_opt {
	char *cstr;
	enum output_mode flag;
};

static const struct flag_opt FLAGS_OPTS[] = {
	{ "none",		OUTPUT_MODE_NONE },
	{ "raw",		OUTPUT_MODE_RAW },
	{ "min",		OUTPUT_MODE_MIN },
	{ "avg",		OUTPUT_MODE_AVG },
	{ "max",		OUTPUT_MODE_MAX },
	{ "moda",		OUTPUT_MODE_MODA },
	{ "median",		OUTPUT_MODE_MEDIAN },
	{ "value",		OUTPUT_MODE_VALUE },
	{ "frequency",		OUTPUT_MODE_FREQUENCY },
	{ "count",		OUTPUT_MODE_COUNT },
	// Mixs
	{ "distribution",	OUTPUT_MIX_DISTRIBUTION},
	{ "all",		OUTPUT_MIX_ALL},
};

static const size_t FLAGS_OPTS_L = sizeof(FLAGS_OPTS) / sizeof(struct flag_opt);

struct data data_create();

int data_parse(struct data *self, const char **argv, int argc, int index);

#endif
