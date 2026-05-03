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
	for (int i = 0; i < flag_opts_l; i++) if (strncasecmp(slice.chars, flag_opts[i].cstr, slice.length) == 0) {
		printf("\t\tsubflag got: %s\n", flag_opts[i].cstr);
		return flag_opts[i].flag;
	}

	fprintf(stderr, ANSI_ERR);
	for (int i = 0; i < slice.length ; i++) putc(slice.chars[i], stderr);
	fprintf(stderr, "is not a valid '--data' flag\n");
	return NONE;
}

// This will split the ';' to mix flags, only this
// just see where the next ';' is, and set this to be the flag
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

	struct string_slice slice = { flags.chars + last_colon_loc, flags.length + last_colon_loc, last_colon_loc};
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

int data_parse(struct data *self, const char **argv, int argc, int index)
{
	if (index + 1 > argc) {

		fprintf(stderr, ANSI_ERR "'--data' option expects at least one flag\n");
		return index;
	}

	struct string *arg = string_new();
	while (index < argc) {

		string_append_cstr(arg, argv[index]);

		int dot_loc = string_find(arg, '.');
		char *arg_cstr = string_to_cstr(arg);

		if (dot_loc == -1) fprintf(stderr, ANSI_ERR "flag '%s' don't have a subflag, indicated by '.'\n", arg_cstr);
		else {
			struct string_slice slice = string_slice(arg, dot_loc + 1, arg->length);

			enum flags flag = parse_flags(slice);

			printf("Brute string: ");
			string_print(arg);
			printf("\nWithout the dot: %s", slice.chars);
			printf("\n\tFrom: %lu", slice.offset);
			printf("\n\tLength: %lu", slice.length);
			printf("\n\tFlag got from: %x", flag);
			puts("\n");
		}

		string_clear(arg);
		index++;
	}
	string_drop(&arg);

	return index - 1;
}

