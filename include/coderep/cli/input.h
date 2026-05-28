#ifndef CODEREP_INPUT_H
#define CODEREP_INPUT_H

#include <stdbool.h>
#include <stddef.h>

struct input {
	const char **files;
	size_t files_l;
	size_t files_c;
};

static const size_t FILES_DEFAULT_CAPACITY = 20;

struct input *input_alloc();

int input_parse(struct input *self, const char **argv, int argc, int index);

void input_drop(struct input **self);

#endif
