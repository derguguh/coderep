#ifndef CODEREP_SOURCE_H
#define CODEREP_SOURCE_H

#include "coderep/data_types/list.h"

#include <stdbool.h>

struct source {
	struct list *files;
};

struct source *source_alloc();

int source_parse(struct source *self, const char **argv, int argc, int index);

void source_drop(struct source **self);

#endif
