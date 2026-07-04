#include "coderep/cli/data.h"

#include "coderep/allocator/allocator.h"
#include "coderep/consts.h"
#include "coderep/data_type/output_mode.h"
#include "coderep/data_type/string.h"
#include "coderep/macros.h"

#include <stdio.h>
#include <string.h>

const struct flag_opt FLAGS_OPTS[] = {
	{ "none", "Disable an type of data", OUTPUT_MODE_NONE },
	{ "raw", "Show the internal structure of the data", OUTPUT_MODE_RAW },
	{ "min", "Show the minimal value", OUTPUT_MODE_MIN },
	{ "avg", "Show the avarage value", OUTPUT_MODE_AVG },
	{ "max", "Show the maximum value", OUTPUT_MODE_MAX },
	{ "mode", "Show the most common value", OUTPUT_MODE_MODE },
	{ "median", "Show the value in middle", OUTPUT_MODE_MEDIAN },
	{ "value", "Show the values (warning: this may output a lot of data)", OUTPUT_MODE_VALUE },
	{ "frequency", "Show the frequency of values", OUTPUT_MODE_FREQUENCY },
	{ "count", "Show the counting of values", OUTPUT_MODE_COUNT },
	// Mixs
	{ "distribution", "Show the values and their frequency (value | frequency)", OUTPUT_MIX_DISTRIBUTION },
	{ "stats", "Show all the data is possible to get with the distribution (min | avg | max | mode | median)", OUTPUT_MIX_STATS },
	{ "all", "Show all the data, including raw structure", OUTPUT_MIX_ALL },
};

const size_t FLAGS_OPTS_L = sizeof(FLAGS_OPTS) / sizeof(struct flag_opt);

enum output_mode parse_flag(const struct string_slice slice)
{
	for (size_t i = 0; i < FLAGS_OPTS_L; i++) if (strncasecmp(slice.chars, FLAGS_OPTS[i].cstr, slice.length) == 0) return FLAGS_OPTS[i].flag;

	fprintf(stderr, ANSI_ERR "\"");
	for (size_t i = 0; i < slice.length ; i++) putc(slice.chars[i], stderr);
	fprintf(stderr, "\" is not a valid '--data' subflag\n" ANSI_ESC ANSI_RESET);

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

struct data data_create()
{
	struct data data = {
		.frequency = OUTPUT_DEFAULT_FREQUENCY,
		.statistic = OUTPUT_DEFAULT_STATISTIC,
		.brute = OUTPUT_DEFAULT_BRUTE,
	};

	return data;
}

// type is a cstr, so no risk with these strncasecmp iirc the C standard
void data_put_in(struct data *self, enum output_mode flag, const struct string_slice type)
{
	enum output_mode *f = NULL;
	if	(strcasecmp("frequency", type.chars) == 0) f = &self->frequency;
	else if (strcasecmp("statistic", type.chars) == 0) f = &self->statistic;
	else if (strcasecmp("brute", type.chars) == 0) f = &self->brute;
	else {
		fprintf(stderr, ANSI_ERR "\"");
		for (size_t i = 0; i < type.length ; i++) putc(type.chars[i], stderr);
		fprintf(stderr, "\"" ANSI_ESC ANSI_RESET "\n");
		return;
	}

	*f = flag;
}

int data_parse(struct data *self, const char **const argv, int argc, int index)
{
	if (index >= argc) {

		PRINT_ERR("'--data' option expects at least one flag");
		return index;
	}
	*self = (struct data) {0};

	struct allocator allocator = default_get_allocator();
	struct string *arg = string_alloc(&allocator, 12);
	while (index < argc) {

		string_append_cstr(arg, argv[index]);
		if (string_get(arg, 0) == '-') break;

		int dot_loc = string_find(arg, '.');
		char *arg_cstr = string_to_cstr(arg);

		if (dot_loc == -1) PRINTF_ERR("flag \"%s\" don't have a subflag, indicated by '.'", arg_cstr);
		else {
			struct string_slice output_mode_slice = string_slice(arg, dot_loc + 1, arg->length);
			struct string_slice type_slice = string_slice(arg, 0, dot_loc);

			data_put_in(self, parse_output_mode(output_mode_slice), type_slice);
		}

		string_clear(arg);
		index++;
	}

	string_drop(&arg);
	return index;
}

