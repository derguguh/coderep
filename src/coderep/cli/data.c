#include "coderep/cli/data.h"

#include "coderep/consts.h"
#include "coderep/data_type/output_mode.h"
#include "coderep/data_type/string.h"
#include "coderep/printer/printer.h"

#include <stdio.h>
#include <string.h>

const struct flag_opt FLAGS_OPTS[] = {
	{ "none", "Disable a data type", OUTPUT_MODE_NONE },
	{ "raw", "Show the internal structure of the data", OUTPUT_MODE_RAW },
	{ "min", "Show the minimal value", OUTPUT_MODE_MIN },
	{ "avg", "Show the average value", OUTPUT_MODE_AVG },
	{ "max", "Show the maximum value", OUTPUT_MODE_MAX },
	{ "mode", "Show the most common value", OUTPUT_MODE_MODE },
	{ "median", "Show the middle value", OUTPUT_MODE_MEDIAN },
	{ "value", "Show the values (warning: this may output a lot of data)", OUTPUT_MODE_VALUE },
	{ "frequency", "Show the frequency of values", OUTPUT_MODE_FREQUENCY },
	{ "count", "Show the count", OUTPUT_MODE_COUNT },
	// Mixs
	{ "distribution", "Show the values and their frequency (value | frequency)", OUTPUT_MIX_DISTRIBUTION },
	{ "stats", "Show all the data that can be derived from the distribution (min | avg | max | mode | median)", OUTPUT_MIX_STATS },
	{ "all", "Show all the data, including raw structure", OUTPUT_MIX_ALL },
};

const size_t FLAGS_OPTS_L = sizeof(FLAGS_OPTS) / sizeof(struct flag_opt);

enum output_mode parse_flag(const struct string_slice slice)
{
	for (size_t i = 0; i < FLAGS_OPTS_L; i++) if (strncasecmp(slice.chars, FLAGS_OPTS[i].name, slice.length) == 0) return FLAGS_OPTS[i].flag;

	printf_err("\"%*s\" is not a valid '--data' flag" ANSI_RESET "\n", (int)slice.length, slice.chars);

	return OUTPUT_MODE_NONE;
}

enum output_mode parse_output_mode(const struct string_slice output_mode)
{
	enum output_mode flag = OUTPUT_MODE_NONE;

	size_t last_colon_loc = 0;
	for (size_t i = 0; i < output_mode.length; i++) {

		if (output_mode.chars[i] == ':') {

			struct string_slice slice = { output_mode.chars + last_colon_loc, i - last_colon_loc, last_colon_loc };
			flag |= parse_flag(slice);
			last_colon_loc = i + 1;
		}
	}

	struct string_slice slice = { output_mode.chars + last_colon_loc, output_mode.length - last_colon_loc, last_colon_loc};
	flag |= parse_flag(slice);

	return flag;
}

// type is a cstr, so no risk with these strncasecmp iirc the C standard
bool data_put_in(struct data *self, enum output_mode flag, const struct string_slice type)
{
	if	(strncasecmp("frequency", type.chars, 9) == 0) self->frequency = flag;
	else if (strncasecmp("statistic", type.chars, 9) == 0) self->statistic = flag;
	else if (strncasecmp("brute", type.chars, 5) == 0) self->brute = flag;
	else {
		printf_err("\"%*s\" is not a valid '--data' group", (int)type.length, type.chars);
		return false;
	}

	return true;
}

bool data_parse(struct config *config, struct argument_list *_list)
{
	config->data = (struct data) {0};

	struct string *arg = string_alloc(config->allocator, 80);
	bool success = true;
	for (size_t i = 0; i < _list->arguments_l; i++) {
		string_append_cstr(arg, _list->arguments[i]);

		int dot_loc = string_find(arg, '.');
		char *arg_cstr = string_to_cstr(arg);

		if (dot_loc == -1) {
			success = false;
			printf_err("group \"%s\" don't have a flag, indicated by '.'", arg_cstr);
		} else {
			struct string_slice output_mode_slice = string_slice(arg, dot_loc + 1, arg->length);
			struct string_slice type_slice = string_slice(arg, 0, dot_loc);

			data_put_in(&config->data, parse_output_mode(output_mode_slice), type_slice);
		}

		string_clear(arg);
	}

	string_drop(&arg);
	return success;
}

