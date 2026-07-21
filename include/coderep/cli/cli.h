#ifndef CODEREP_CLI_CLI_H
#define CODEREP_CLI_CLI_H

#include "coderep/cli/option.h"
#include <stdbool.h>
#include <stddef.h>

struct cli;

extern const struct option OPTIONS[];

extern const size_t OPTIONS_L;

struct cli *cli_alloc(struct allocator *allocator);

const struct config *cli_parse_args(struct cli *self, const char **argv, size_t argc);

void cli_drop(struct cli **self);

#endif
