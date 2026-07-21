#ifndef CODEREP_CLI_QUIET_H
#define CODEREP_CLI_QUIET_H

#include "coderep/cli/config.h"
#include "coderep/cli/option.h"

bool quiet_set(struct config *config, struct argument_list *ignore_)
{
	config->verbosity = QUIET;
	return true;
}

#endif
