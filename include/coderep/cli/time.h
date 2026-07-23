#ifndef CODEREP_CLI_TIME_H
#define CODEREP_CLI_TIME_H

#include "coderep/cli/option.h"

bool time_set(struct config *config, struct argument_list *ignore_)
{
	config->show_time = true;
	return true;
}

#endif
