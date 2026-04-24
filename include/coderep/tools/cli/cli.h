#ifndef CODERE_TOOLS_CLI_CLI_H
#define CODERE_TOOLS_CLI_CLI_H

#include "coderep/tools/cli/data.h"
#include "coderep/tools/cli/filter.h"

struct cli {
	struct data data;
	struct filter filter;
};

#endif
