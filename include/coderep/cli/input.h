#ifndef CODEREP_INPUT_H
#define CODEREP_INPUT_H

#include "coderep/data_types/list.h"

#include <stdbool.h>

struct input {
	struct list *files;
};

struct input *input_alloc();

int input_parse(struct input *self, const char **argv, int argc, int index);

void input_drop(struct input **self);

#endif
