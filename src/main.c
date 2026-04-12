#include "coderep/data_types/distributive.h"
#include "coderep/data_types/string.h"

int main(int argc, char **argv)
{
	struct string *name = string_new();
	string_append_cstr(name, "just.an.exemple");
	struct string *value = string_new();
	string_append_cstr(value, "foo");
	struct distributive *distributive = distributive_new(&name, 0);
	
	distributive_push(distributive, &value);

	for (int i = 0; i < 5; i++) {
		value = string_new();
		string_append_cstr(value, "foo");
		distributive_push(distributive, &value);
	}
	for (int i = 0; i < 12; i++) {
		value = string_new();
		string_append_cstr(value, "idk");
		distributive_push(distributive, &value);
	}
	for (int i = 0; i < 2; i++) {
		value = string_new();
		string_append_cstr(value, "hello!");
		distributive_push(distributive, &value);
	}

	distributive_print(distributive);

	distributive_drop(&distributive);
}

