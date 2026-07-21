#ifndef CODEREP_DATA_H
#define CODEREP_DATA_H

#include "coderep/cli/option.h"
#include "coderep/data_type/output_mode.h"

#include <stddef.h>

struct flag_opt {
	const char *const name;
	const char *const description;
	enum output_mode flag;
};

extern const struct flag_opt FLAGS_OPTS[];

extern const size_t FLAGS_OPTS_L;

bool data_parse(struct config *config, struct argument_list *_list);

#endif
