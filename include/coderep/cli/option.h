#ifndef CODEREP_CLI_OPTION_H
#define CODEREP_CLI_OPTION_H

#include <stdbool.h>
#include <stddef.h>

struct option_state {
	bool stop_execution;
	bool success;
};

struct argument_list {
	const char **arguments;
	size_t arguments_l;
};

typedef struct option_state (*option_parse_fn) (void *structure, struct argument_list *list);

enum entry_type {
	COMPOSABLE,
	ONE_INPUT,
	MULTIPLE_INPUT,
};

struct option {
	void *structure;
	const char *flag_name;
	char short_name;
	option_parse_fn parsing;
	enum entry_type type;
};

#endif
