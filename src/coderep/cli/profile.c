#include "coderep/cli/profile.h"
#include "coderep/macros.h"
#include <string.h>

const struct profile_opt PROFILE_OPT[] = {
	{ "one", "less than the minimal", 1 },
	{ "minimal", "the minimal", 16 },
	{ "feather", "each data will be light as a feather", 64 },
	{ "light", "each data will be light", 256 },
	{ "medium", "(default) the good balance", 1024 },
	{ "heavy", "will use more buckets than normal", 2048 },
	{ "lead", "heavy as lead", 4096 },
	{ "extreme", "too much buckets, more data, more memory", 16384 },
	{ "limit", "this is the limit that Code Report is able to pre-allocate", 65536 },
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
	PRINTF_ERR("'%s' is not a valid profile", _list->arguments[0]);
	return false;
}

