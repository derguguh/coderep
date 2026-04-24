#include "coderep/tools/cli/data.h"
#include "coderep/consts.h"

#include <stdio.h>

struct data data_create()
{
	struct data data = {
		DEFAULT_DISTRIBUTIVE,
		DEFAULT_BRUTE,
		DEFAULT_LIST,
	};

	return data;
}

enum flags parse_flags(char **argv, int argc, int index)
{
	
}

int data_parse(struct data *self, char **argv, int argc, int index)
{
	if (index + 1 > argc) {

		fprintf(stderr, ANSI_ERR "'--data' option expects at least one flag");
		return index;
	}
	index++;

	while ()

	return index - 1;
}
