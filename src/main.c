#include "coderep/tools/argument_parser.h"

int main(int argc, char **argv)
{
	struct args *args = args_new(argv, argc);

	printf("language: ");
	string_print(args->lang);
	putchar('\n');
	printf("Filter: ");
	string_print(args->filter);
	putchar('\n');

	list_print(args->sources);
	list_print(args->inputs);

	args_drop(&args);
}

