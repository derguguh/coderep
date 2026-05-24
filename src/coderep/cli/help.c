#include "coderep/cli/help.h"
#include "coderep/cli/data.h"

#include <stdio.h>

int help_print(void *_ignore, const char **const argv, int argc, int index)
{
	printf("-- Help menu start --\n\n");

	printf("\t--help: show this menu\n");
	printf("\t--lang: select the language backend (a compiler front-end)\n");
	printf("\t--input: add a file to the analysis\n");
	printf("\t--source: search files in a folder\n");

	printf("\t--filter: add a filter to what data to output\n");
	printf("\t\tFilter flag is not fully implemented yet\n\n");

	printf("\t--data: changes how the data types should output\n");
	printf("\t\tExist 'frequency', 'list' and 'brute'\n");
	printf("\t\tEach one needs a flag that show how to output the data, and this can be selected with '.'\n");
	printf("\t\tYou can also mix one more flags with ':'\n");
	printf("\t\tFor example, 'frequency.distribution:avg' shows the distribution and the average value\n");
	printf("\t\tthe current flags are: ");

	printf("%s", FLAGS_OPTS[0].cstr);
	for (size_t i = 1; i < FLAGS_OPTS_L; i++) printf(", %s", FLAGS_OPTS[i].cstr);

	putchar('\n');

	printf("\n-- Help menu end --\n");

	return index;
}

