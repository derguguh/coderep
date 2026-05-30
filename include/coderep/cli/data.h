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

extern const struct flag_opt FLAGS_OPTS[];

extern const size_t FLAGS_OPTS_L;

struct data data_create();

int data_parse(struct data *self, const char **argv, int argc, int index);

#endif
