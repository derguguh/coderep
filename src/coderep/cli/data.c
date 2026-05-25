#include "coderep/cli/data.h"
#include "coderep/consts.h"
#include "coderep/data_types/string.h"
#include "coderep/macros.h"

#include <stdio.h>
#include <string.h>

enum flags parse_flag(const struct string_slice slice)
{
	for (int i = 0; i < FLAGS_OPTS_L; i++) if (strncasecmp(slice.chars, FLAGS_OPTS[i].cstr, slice.length) == 0) return FLAGS_OPTS[i].flag;

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
		.frequency=DEFAULT_FREQUENCY,
		.brute=DEFAULT_BRUTE,
		.list=DEFAULT_LIST,
	};

	return data;
}

// type is a cstr, so no risk with these strncasecmp iirc the C standard
void data_put_in(struct data *self, enum flags flag, const struct string_slice type)
{
	enum flags *f = NULL;
	if	(strncasecmp("frequency", type.chars, 9) == 0) f = &self->frequency;
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

int data_parse(struct data *self, const char **const argv, int argc, int index)
{
	if (index >= argc) {

		PRINT_ERR("'--data' option expects at least one flag");
		return index;
	}
	self->frequency = NONE;
	self->brute = NONE;
	self->list = NONE;

	struct string *arg = string_alloc();
	while (index < argc) {

		string_append_cstr(arg, argv[index]);
		if (string_get(arg, 0) == '-') break;

		int dot_loc = string_find(arg, '.');
		char *arg_cstr = string_to_cstr(arg);

		if (dot_loc == -1) PRINTF_ERR("flag \"%s\" don't have a subflag, indicated by '.'", arg_cstr);
		else {
			struct string_slice flags_slice = string_slice(arg, dot_loc + 1, arg->length);
			struct string_slice type_slice = string_slice(arg, 0, dot_loc);

			data_put_in(self, parse_flags(flags_slice), type_slice);
		}

		string_clear(arg);
		index++;
	}

	string_drop(&arg);
	return index;
}

