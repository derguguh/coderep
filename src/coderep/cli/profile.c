#include "coderep/cli/profile.h"

#include "coderep/tool/printer/printer.h"

#include <string.h>

const struct profile_opt PROFILE_OPT[] = {
	{ "one",	"lowest budget the tool can operate with", 1 },
	{ "minimal",	"minimal practical budget", 16 },
	{ "feather",	"budget light as feather", 64 },
	{ "light",	"light budget", 256 },
	{ "medium",	"balance (default)", 1024 },
	{ "heavy",	"heavier budget", 2048 },
	{ "lead",	"budget heavy as lead", 4096 },
	{ "extreme",	"budget for workloads where lead is too light", 16384 },
	{ "limit",	"highest budget the tool can pre-allocate", 65536 },
};

const size_t PROFILE_OPT_L = sizeof(PROFILE_OPT) / sizeof(struct profile_opt);

bool profile_parse(struct config *config, struct argument_list *_list)
{
	for (size_t i = 0; i < PROFILE_OPT_L; i++) {
		if (strcmp(PROFILE_OPT[i].name, _list->arguments[0]) == 0) {
			config->bucket_baseline = PROFILE_OPT[i].bucket_baseline;
			return true;
		}
	}
	printf_err("'%s' is not a valid profile", _list->arguments[0]);
	return false;
}

