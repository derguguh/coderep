#ifndef CODEREP_FILTER_H
#define CODEREP_FILTER_H

#include <stdbool.h>

struct filter {
	bool all;
};

struct filter filter_create();

int filter_parse(struct filter *self, const char **argv, int argc, int index);

#endif
