#ifndef CODEREP_CLI_CLI_H
#define CODEREP_CLI_CLI_H

#include <stdbool.h>
#include <stddef.h>

struct cli_options {
	bool quiet;
	bool timer;
};

struct cli;

struct cli *cli_alloc();

void cli_parse_args(struct cli *self, const char **argv, size_t argc);

void cli_drop(struct cli **self);

#endif
