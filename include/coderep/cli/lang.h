#ifndef CODEREP_LANG_H
#define CODEREP_LANG_H

#include <stdbool.h>

struct lang {
	struct string *file;
};

struct lang lang_create();

int lang_parse(struct lang *self, const char **argv, int argc, int index);

#endif
