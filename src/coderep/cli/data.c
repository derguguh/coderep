#include "coderep/cli/data.h"

#include "coderep/allocator/allocator.h"
#include "coderep/consts.h"
#include "coderep/data_type/output_mode.h"
#include "coderep/data_type/string.h"
#include "coderep/macros.h"

#include <stdio.h>
#include <string.h>

const struct flag_opt FLAGS_OPTS[] = {
	{ "none",		OUTPUT_MODE_NONE },
	{ "raw",		OUTPUT_MODE_RAW },
	{ "min",		OUTPUT_MODE_MIN },
	{ "avg",		OUTPUT_MODE_AVG },
	{ "max",		OUTPUT_MODE_MAX },
	{ "mode",		OUTPUT_MODE_MODE },
	{ "median",		OUTPUT_MODE_MEDIAN },
	{ "value",		OUTPUT_MODE_VALUE },
	{ "frequency",		OUTPUT_MODE_FREQUENCY },
	{ "count",		OUTPUT_MODE_COUNT },
	// Mixs
	{ "distribution",	OUTPUT_MIX_DISTRIBUTION},
	{ "stats",		OUTPUT_MIX_STATS},
	{ "all",		OUTPUT_MIX_ALL},
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
		.brute = OUTPUT_DEFAULT_BRUTE,
	};

	return data;
}

// type is a cstr, so no risk with these strncasecmp iirc the C standard
void data_put_in(struct data *self, enum output_mode flag, const struct string_slice type)
{
	enum output_mode *f = NULL;
	if	(strncasecmp("frequency", type.chars, 9) == 0) f = &self->frequency;
	else if (strncasecmp("brute", type.chars, 5) == 0) f = &self->brute;
	//else if (strncasecmp("list", type.chars, 4) == 0) f = &self->list; // list removed, should have bimap in the future
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
	self->frequency = OUTPUT_MODE_NONE;
	self->brute = OUTPUT_MODE_NONE;

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

