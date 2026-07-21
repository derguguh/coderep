#ifndef CODEREP_CLI_VERBOSE_H
#define CODEREP_CLI_VERBOSE_H

#include "coderep/cli/config.h"
#include "coderep/cli/option.h"

bool verbose_set(struct config *config, struct argument_list *ignore_)
{
	config->verbosity = VERBOSE;
	return true;
}

#endif
