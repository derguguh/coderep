#include "coderep/cli/filter.h"

struct filter filter_create()
{
	struct filter filter = {
		.all = true
	};

	return filter;
}

int filter_parse(struct filter *self, const char **argv, int argc, int index)
{
	return index;
}
