#ifndef CODEREP_CLI_COLOR_H
#define CODEREP_CLI_COLOR_H

#include "coderep/cli/config.h"
#include "coderep/cli/option.h"

bool color_set(struct config *config, struct argument_list *ignore_)
{
	config->color = true;
}

#endif
