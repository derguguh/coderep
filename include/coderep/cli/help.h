#ifndef CODEREP_HELP_H
#define CODEREP_HELP_H

#include "coderep/cli/option.h"

struct option help_entry();

struct option_state help_print(void *null, struct argument_list *ignore);

#endif
