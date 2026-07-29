#ifndef CODEREP_CLI_CONFIG_H
#define CODEREP_CLI_CONFIG_H

#include "coderep/allocator/allocator.h"
#include "backend/backend.h"
#include "coderep/data_type/output_mode.h"

#include <stdbool.h>
#include <stddef.h>

enum verbosity {
	QUIET,
	NORMAL,
	VERBOSE,
};

struct config {
	struct allocator *allocator;
	const struct backend *_backend;
	struct string **paths;
	struct string *filter;
	struct data data;
	size_t bucket_baseline;
	size_t paths_l;
	size_t paths_c;
	enum verbosity verbosity;
	bool do_color;
	bool show_time;
	bool procced;
};

static const size_t FILES_DEFAULT_CAPACITY = 20;

void config_input_expand(struct config *config);

#endif
