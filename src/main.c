#include "coderep/tools/cli/cli.h"

int main(int argc, char **argv)
{
	struct data data = data_create();

	data_parse(&data, argv, argc, 2);
}

