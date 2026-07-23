#include "coderep/cli/cli.h"

#include "coderep/allocator/allocator.h"
#include "coderep/cli/color.h"
#include "coderep/cli/config.h"
#include "coderep/cli/data.h"
#include "coderep/cli/help.h"
#include "coderep/cli/input.h"
#include "coderep/cli/lang.h"
#include "coderep/cli/option.h"
#include "coderep/cli/profile.h"
#include "coderep/cli/quiet.h"
#include "coderep/cli/time.h"
#include "coderep/cli/verbose.h"
#include "coderep/data_type/output_mode.h"
#include "coderep/macros.h"

#include <stdlib.h>
#include <string.h>

struct cli {
	struct config *config;
	struct option *entries;
	size_t entries_l;
};

const struct option OPTIONS[] = {
	{ "help",	'h', help_print,	COMPOSABLE },
	{ "quiet",	'q', quiet_set,		COMPOSABLE },
	{ "verbose",	'v', verbose_set,	COMPOSABLE },
	{ "color",	'c', color_set,		COMPOSABLE },
	{ "time",	't', time_set,		COMPOSABLE },
	{ "lang",	'l', lang_parse,	ONE_INPUT },
	{ "profile",	'p', profile_parse,	ONE_INPUT },
	{ "input",	'i', input_parse,	MULTIPLE_INPUT },
	{ "data",	'd', data_parse,	MULTIPLE_INPUT },
};

const size_t OPTIONS_L = ARRAY_SIZE(OPTIONS, struct option);

struct cli *cli_alloc(struct allocator *allocator)
{
	struct option *entries = allocator->allocate(allocator->state, sizeof(struct option) * OPTIONS_L);
	struct config *config = allocator->allocate(allocator->state, sizeof(struct config));
	struct string **paths = allocator->allocate(allocator->state, sizeof(struct string *) * FILES_DEFAULT_CAPACITY);
	struct cli *self = allocator->allocate(allocator->state, sizeof(struct cli));

	if (!entries || !config || !paths || !self) goto defer;

	memcpy(entries, OPTIONS, sizeof(OPTIONS));
	*config = (struct config) {
		.allocator = allocator,
		.data = (struct data) {
			.frequency = OUTPUT_DEFAULT_FREQUENCY,
			.statistic = OUTPUT_DEFAULT_STATISTIC,
			.brute = OUTPUT_DEFAULT_BRUTE,
		},
		.verbosity = NORMAL,
		.bucket_baseline = 1024,
		.paths = paths,
		.paths_c = FILES_DEFAULT_CAPACITY,
		.paths_l = 0,
		.procced = true,
	};
	*self = (struct cli) {
		.config = config,
		.entries = entries,
		.entries_l = OPTIONS_L,
	};

	return self;
defer:
	ALLOC_ERR_EXIT("struct cli");
}

struct argument_list get_args(const char **argv, size_t argc)
{
	size_t counting = 0;
	for (size_t i = 1; i < argc; i++) {
		if (argv[i][0] == '-') break;
		else counting++;
	}

	return (struct argument_list) {
		.arguments = argv + 1, 
		.arguments_l = counting,
	};
}

struct option *cli_get_option(struct cli *self, const char *arg, size_t index, bool long_)
{
	if (long_) {
		for (size_t i = 0; i < self->entries_l; i++) {
			if (strcmp(arg + 2, self->entries[i].flag_name) == 0) return &self->entries[i];
		}
	} else {
		char c = arg[index];
		for (size_t i = 0; i < self->entries_l; i++) {
			if (c == self->entries[i].short_name) return &self->entries[i];
		}
	}

	return NULL;
}

void cli_handle_composition(struct cli *self, const char *arg)
{
	size_t index = 2;
	char c = arg[index];
	while (c != '\0') {
		struct option *option = cli_get_option(self, arg, index, false);
		if (option == NULL) {
			PRINTF_ERR("'%c' is not a known composable flag", c);
			self->config->procced = false;
		} else if (option->type == COMPOSABLE) {
			if (!option->parsing(self->config, NULL)) self->config->procced = false;
		} else {
			PRINTF_ERR("'%c' is not composable, you can't write as '%s'", c, arg);
			self->config->procced = false;
		}

		index++;
		c = arg[index];
	}
}

size_t cli_handle_option(struct cli *self, struct option *option, const char *arg, const char **argv, size_t argc, bool long_)
{
	if (option->type == COMPOSABLE) {
		if (!option->parsing(self->config, NULL)) self->config->procced = false;
		if (!long_) cli_handle_composition(self, arg);
		return 0;
	} else {
		if (argc < 2) {
			PRINTF_ERR("'%s' needs at least one argument", arg);
			self->config->procced = false;
			return 0;
		}
		struct argument_list list;
		if (option->type == ONE_INPUT) {
			list = (struct argument_list) {
				.arguments = argv + 1,
				.arguments_l = 1,
			};
		} else list = get_args(argv, argc);
		if (!option->parsing(self->config, &list)) self->config->procced = false;

		return list.arguments_l;
	}
}

size_t cli_parse_arg(struct cli *self, const char *arg, const char **argv, size_t argc)
{
	size_t arg_l = strlen(arg);
	
	if (arg_l < 2 || arg[0] != '-') {
		PRINTF_ERR("'%s' is not handled by Code Report CLI", arg);
		self->config->procced = false;
		return 0;
	}

	bool long_ = arg[1] == '-';
	struct option *option = cli_get_option(self, arg, 1, long_);
	if (option == NULL) {
		PRINTF_ERR("'%s' is not handled by Code Report CLI", arg);
		self->config->procced = false;
		return 0;
	}

	return cli_handle_option(self, option, arg, argv, argc, long_);
}

const struct config *cli_parse_args(struct cli *self, const char **argv, size_t argc)
{
	for (size_t i = 1; i < argc; i++) i += cli_parse_arg(self, argv[i], argv + i, argc - i);
	return self->config;
}

void cli_drop(struct cli **self)
{
	if (!self || !(*self)) return;
	struct cli *cli = *self;
	struct allocator *allocator = cli->config->allocator;

	for (size_t i = 0; i < cli->config->paths_l; i++) string_drop(&cli->config->paths[i]);
	allocator->deallocate(allocator->state, cli->config->paths);
	cli->config->paths = NULL;
	allocator->deallocate(allocator->state, cli->config);
	cli->config = NULL;
	allocator->deallocate(allocator->state, cli->entries);
	cli->entries = NULL;
	allocator->deallocate(allocator->state, *self);
	*self = NULL;
}

