#ifndef CODEREP_CLI_CLI_H
#define CODEREP_CLI_CLI_H

#include "coderep/cli/data.h"
#include "coderep/cli/filter.h"

struct cli {
	struct data data;
	struct filter filter;
};

#endif
