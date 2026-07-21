#ifndef CODEREP_CLI_PROFILE_H
#define CODEREP_CLI_PROFILE_H

#include "coderep/cli/option.h"

struct profile_opt {
	const char *const name;
	const char *const description;
	size_t bucket_baseline;
};

extern const struct profile_opt PROFILE_OPT[];

extern const size_t PROFILE_OPT_L;

bool profile_parse(struct config *config, struct argument_list *_list);

#endif
