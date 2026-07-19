#include "coderep/cli/help.h"

#include "backend/backend.h"
#include "coderep/cli/data.h"
#include "coderep/cli/option_entry.h"

#include <stdio.h>
#include <string.h>

#define SPACE "    "

void print_backend(const struct backend *backend, const struct backend *_next, bool start, bool end)
{
	bool sub_flag = strchr(backend->identifier, '.') != NULL;
	bool is_next_sub_flag = _next == NULL ? false : strchr(_next->identifier, '.') != NULL;

	printf(SPACE SPACE);
	if (sub_flag) {
		if (start)	printf("┌─");
		else if (end)	printf("└─");
		else		printf("│ ");
	} else {
		if (start)	printf("┌─");
		else if (end)	printf("└─");
		else		printf("├─");
	}

	if (sub_flag) {
		if (is_next_sub_flag) printf("├─%-18.20s", backend->name);
		else printf("└─%-18.20s", backend->name);
	} else printf("%-20.20s", backend->name);

	printf("%-20.20s", backend->identifier);
	printf("%s\n", backend->description);
}

struct option_state help_print(void *null, struct argument_list *ignore)
{
	printf("-- Help menu start --\n\n");

	printf(SPACE "--help: show this menu\n");
	printf(SPACE "--input: add files to the analysis\n\n");

	printf(SPACE "--filter: add a filter to what data to output\n");
	printf(SPACE SPACE "Filter flag is not implemented yet\n\n");

	printf(SPACE "--lang: select the language backend (a compiler front-end)\n");
	printf(SPACE SPACE "The current backends are:\n\n");

	for (size_t i = 0; i < BACKEND_REGISTER_L - 1; i++) print_backend(&BACKEND_REGISTER[i], &BACKEND_REGISTER[i + 1], i == 0, false);
	print_backend(&BACKEND_REGISTER[BACKEND_REGISTER_L - 1], NULL, false, true);
	printf("\n\n");

	printf(SPACE "--data: changes how the data types should output\n");
	printf(SPACE SPACE "The current flags are:\n\n");

	for (size_t i = 0; i < FLAGS_OPTS_L; i++) {
		printf(SPACE SPACE "");
		if (i == 0) printf("┌─");
		else if (i < FLAGS_OPTS_L - 1) printf("├─");
		else printf("└─");
		printf("%-20.20s", FLAGS_OPTS[i].name);
		printf("%s\n", FLAGS_OPTS[i].description);
	}
	printf("\n\n");

	printf("-- Help menu end --\n");

	return (struct option_state) {};
}

