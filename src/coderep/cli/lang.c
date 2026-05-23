#include "coderep/cli/lang.h"

struct lang lang_create()
{
	struct lang lang = {

	};

	return lang;
}

int lang_parse(struct lang *self, const char **argv, int argc, int index)
{
	return index;
}

