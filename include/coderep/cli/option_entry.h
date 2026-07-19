#ifndef CODEREP_CLI_CLI_H
#define CODEREP_CLI_CLI_H

#include <stdbool.h>
#include <stddef.h>

struct option_state {
	bool stop_execution;
	bool success;
};

struct argument_list {
	struct string **arguments;
	size_t arguments_l;
};

typedef struct option_state (*option_parse_t) (void *structure, struct argument_list *list);

enum entry_type {
	COMPOSABLE,
	ONE_INPUT,
	MULTIPLE_INPUT,
};

struct option_entry {
	void *structure;
	const char *const flag_name;
	char short_name;
	option_parse_t parsing;
	enum entry_type type;
};

struct option_table {
	struct option_entry *entries;
	size_t entries_l;
};

#endif
