#include "coderep/tools/cli/data.h"
#include "coderep/consts.h"
#include "coderep/data_types/string.h"

#include <stdio.h>
#include <string.h>

struct flag_opt {
	char *cstr;
	enum flags flag;
};

const struct flag_opt flag_opts[] = {
	{ "none",		NONE },
	{ "raw",		RAW },
	{ "min",		MIN },
	{ "avg",		AVG },
	{ "max",		MAX },
	{ "moda",		MODA },
	{ "median",		MEDIAN },
	{ "value",		VALUE },
	{ "frequency",		FREQUENCY },
	{ "count",		COUNT },
	// Mixs
	{ "distribution",	DISTRIBUTION},
	{ "all",		ALL},
};

const size_t flag_opts_l = sizeof(flag_opts) / sizeof(struct flag_opt);

enum flags parse_flag(const struct string_slice slice)
{
	for (int i = 0; i < flag_opts_l; i++) if (strncasecmp(slice.chars, flag_opts[i].cstr, slice.length) == 0) return flag_opts[i].flag;
	fprintf(stderr, ANSI_ERR "\"");
	for (int i = 0; i < slice.length ; i++) putc(slice.chars[i], stderr);
	fprintf(stderr, "\" is not a valid '--data' subflag\n" ANSI_ESC ANSI_RESET);
	return NONE;
}

enum flags parse_flags(const struct string_slice flags)
{
	enum flags flag = NONE;

	size_t last_colon_loc = 0;
	for (size_t i = 0; i < flags.length; i++) {

		if (flags.chars[i] == ':') {

			struct string_slice slice = { flags.chars + last_colon_loc, i - last_colon_loc, last_colon_loc };
			flag |= parse_flag(slice);
			last_colon_loc = i + 1;
		}
	}

	struct string_slice slice = { flags.chars + last_colon_loc, flags.length - last_colon_loc, last_colon_loc};
	flag |= parse_flag(slice);

	return flag;
}

struct data data_create()
{
	struct data data = {
		DEFAULT_DISTRIBUTIVE,
		DEFAULT_BRUTE,
		DEFAULT_LIST,
	};

	return data;
}

// type is a cstr, so no risk with these strncasecmp iirc the C standard
void data_put_in(struct data *self, enum flags flag, const struct string_slice type)
{
	enum flags *f = NULL;
	if	(strncasecmp("distributive", type.chars, 12) == 0) f = &self->distributive;
	else if (strncasecmp("brute", type.chars, 5) == 0) f = &self->brute;
	else if (strncasecmp("list", type.chars, 4) == 0) f = &self->list;
	else {
		fprintf(stderr, ANSI_ERR "\"");
		for (int i = 0; i < type.length ; i++) putc(type.chars[i], stderr);
		fprintf(stderr, "\"" ANSI_ESC ANSI_RESET "\n");
		return;
	}

	*f = flag;
}

int data_parse(struct data *self, const char **argv, int argc, int index)
{
	if (index + 1 > argc) {

		PRINT_ERR("'--data' option expects at least one flag\n");
		return index;
	}
	self->distributive = NONE;
	self->brute = NONE;
	self->list = NONE;

	struct string *arg = string_new();
	while (index < argc) {

		string_append_cstr(arg, argv[index]);

		int dot_loc = string_find(arg, '.');
		char *arg_cstr = string_to_cstr(arg);

		if (dot_loc == -1) PRINTF_ERR("flag '%s' don't have a subflag, indicated by '.'\n", arg_cstr);
		else {
			struct string_slice flags_slice = string_slice(arg, dot_loc + 1, arg->length);
			struct string_slice type_slice = string_slice(arg, 0, dot_loc);

			data_put_in(self, parse_flags(flags_slice), type_slice);
		}

		string_clear(arg);
		index++;
	}
	string_drop(&arg);
	return index - 1;
}

