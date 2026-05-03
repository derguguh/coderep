#include "coderep/tools/cli/data.h"

int main(int argc, const char **argv)
{
	struct data data = data_create();

	data_parse(&data, argv, argc, 1);
}

