#include "coderep/cli/lang.h"

#include <stdio.h>

struct lang lang_create()
{
	struct lang lang = {

	};

	return lang;
}

int lang_parse(struct lang *self, const char **argv, int argc, int index)
{
	printf("'--lang' option is on WIP! Expect bugs\n");

	return index;
}

