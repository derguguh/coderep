#ifndef CODEREP_LANG_H
#define CODEREP_LANG_H

#include "backend/backend.h"

#include <stdbool.h>

struct lang {
	const struct backend *_backend;
};

struct lang lang_create();

int lang_parse(struct lang *self, const char **argv, int argc, int index);

#endif
