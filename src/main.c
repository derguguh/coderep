#include "coderep/data_types/brute.h"
#include "coderep/data_types/string.h"

int main(int argc, char **argv)
{
	struct string *name = string_new();
	struct string *value = string_new();
	string_append_cstr(name, "language.version");
	string_append_cstr(value, "C99");

	struct brute *brute = brute_new(&name, &value);

	brute_print(brute);

	brute_drop(&brute);
}

