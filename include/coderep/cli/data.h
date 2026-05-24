#ifndef CODEREP_DATA_H
#define CODEREP_DATA_H

// This define WHAT to show
#include <stddef.h>
enum flags {
	// Flags
	NONE			= 0x00000000,
	RAW			= 0x00000001,
	MIN			= 0x00000002,
	AVG			= 0x00000004,
	MAX			= 0x00000008,
	MODA			= 0x00000010,
	MEDIAN			= 0x00000020,
	VALUE			= 0x00000040,
	FREQUENCY		= 0x00000080,
	COUNT			= 0x00000100,
	// Flag mixes
	DEFAULT_BRUTE		= 0x00000040,
	DEFAULT_LIST		= 0x00000040,
	DEFAULT_FREQUENCY	= 0x0000000E,
	DISTRIBUTION		= 0x000000C0,
	STATS			= 0x0000003E,
	ALL			= 0xFFFFFFFF,
};

struct data {
	enum flags frequency;
	enum flags brute;
	enum flags list;
};

struct flag_opt {
	char *cstr;
	enum flags flag;
};

static const struct flag_opt FLAGS_OPTS[] = {
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

static const size_t FLAGS_OPTS_L = sizeof(FLAGS_OPTS) / sizeof(struct flag_opt);

struct data data_create();

int data_parse(struct data *self, const char **argv, int argc, int index);

#endif
