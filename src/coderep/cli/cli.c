#include "coderep/cli/cli.h"

#include "coderep/cli/help.h"
#include "coderep/cli/option.h"
#include "coderep/macros.h"

#include <stdlib.h>
#include <string.h>

struct cli {
	struct option *entries;
	size_t entries_l;
};

const size_t OPTION_AMMOUNT = 1;

struct cli *cli_alloc()
{
	struct option *entries = calloc(sizeof(struct option), OPTION_AMMOUNT);
	struct cli *self = malloc(sizeof(struct cli));
	if (!self) goto defer;

	entries[0] = help_entry();

	*self = (struct cli) {
		.entries = entries,
		.entries_l = OPTION_AMMOUNT,
	};

	return self;
defer:
	ALLOC_ERR_EXIT("struct cli");
}

struct argument_list get_args(const char **argv, size_t argc)
{
	size_t counting = 0;
	for (size_t i = 0; i < argc; i++) {
		if (argv[i][0] == '-') break;
		else counting++;
	}

	return (struct argument_list) {
		argv, 
		counting
	};
}

void cli_handle_composable_option(struct cli *self, const char *arg)
{
	size_t index = 2;
	char c = arg[index];
	while (c != '\0') {
		bool parsed = false;
		for (size_t i = 0; i < self->entries_l; i++) {
			struct option option = self->entries[i];
			if (c == option.short_name) {
				if (option.type != COMPOSABLE) {
					PRINTF_ERR("'%c' is not composable, you can't write as '%s'", c, arg);
					return;
				}
				option.parsing(option.structure, NULL);
				parsed = true;
				break;
			}
		}
		if (!parsed) {
			PRINTF_ERR("'%c' is not a known composable flag", c);
		}
		index++;
		c = arg[index];
	}
}

void cli_handle_short_option(struct cli *self, struct option *option, const char *arg, const char **argv, size_t argc)
{
	if (option->type == COMPOSABLE) {
		option->parsing(option->structure, NULL);
		cli_handle_composable_option(self, arg);
	} else {
		if (argc < 2) {
			PRINTF_ERR("'%s' needs at least one argument", arg);
			return;
		}
		struct argument_list list;
		if (option->type == ONE_INPUT) {
			list = (struct argument_list) {
				.arguments = argv + 1,
				.arguments_l = 1,
			};
		} else list = get_args(argv, argc);
		option->parsing(option->structure, &list);
	}
}

void cli_handle_short_options(struct cli *self, const char *arg, const char **argv, size_t argc)
{
	char c = arg[1];

	for (size_t i = 0; i < self->entries_l; i++) {
		struct option option = self->entries[i];
		if (c == option.short_name) cli_handle_short_option(self, &option, arg, argv, argc);
	}
}

void cli_parse_arg(struct cli *self, const char *arg, const char **argv, size_t argc)
{
	size_t arg_l = strlen(arg);
	
	if (arg_l < 2 || arg[0] != '-') {
		PRINTF_ERR("'%s' is not handled by Code Report CLI", arg);
		return;
	}

	if (arg[1] == '-') {
		for (size_t i = 0; i < self->entries_l; i++) {
			struct option option = self->entries[i];
			if (strcmp(arg + 2, option.flag_name) == 0) {
				struct argument_list list = get_args(argv, argc);
				option.parsing(option.structure, &list);
				break;
			}
		}
	} else {
		cli_handle_short_options(self, arg, argv, argc);
	}
}

void cli_parse_args(struct cli *self, const char **argv, size_t argc)
{
	for (size_t i = 1; i < argc; i++) cli_parse_arg(self, argv[i], argv + i, argc - i);
}

void cli_drop(struct cli **self)
{
	if (!self || !(*self)) return;

	free(*self);
	*self = NULL;
}

