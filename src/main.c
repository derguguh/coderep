#include "coderep/cli/cli.h"

#include <stdbool.h>

int main(int argc, const char **argv)
{
	struct cli *cli = cli_alloc();

	cli_parse(cli, argv, argc);

	cli_drop(&cli);
}

