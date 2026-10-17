#include "coderep/cli/help.h"

#include "backend/backend.h"
#include "coderep/cli/data.h"
#include "coderep/cli/profile.h"

#include <stdio.h>
#include <string.h>

#define SPACE "    "

void print_backend(const struct backend *backend, const struct backend *_next, bool start, bool end)
{
	bool sub_flag = strchr(backend->identifier, '.') != NULL;
	bool is_next_sub_flag = _next == NULL ? false : strchr(_next->identifier, '.') != NULL;

	printf(SPACE SPACE);
	if (sub_flag) {
		if (start)	printf("  ┌─");
		else		printf("  │ ");

		if (is_next_sub_flag) printf("├─%-18.20s", backend->name);
		else printf("└─%-18.20s", backend->name);
	} else {
		if (start)	printf("  ┌─");
		else if (end)	printf("  └─");
		else		printf("  ├─");
		printf("%-20.20s", backend->name);
	}

	printf("%-20.20s", backend->identifier);
	printf("%s\n", backend->description);
}

bool help_print(struct config *ignore, struct argument_list *ignore_)
{
	printf("-- Help menu start --\n\n");

	printf(SPACE "--help: show this menu\n");
	printf(SPACE "--input: add files to the analysis\n");
	printf(SPACE "--verbose: adds more info to stdout\n");
	printf(SPACE "--quiet: remove some output from stdout\n");
	printf(SPACE "--time: adds some data about time of internal work\n");
	printf(SPACE "--color: adds color to the output\n");

	printf("\n"  SPACE "--filter: add a filter to what data to output\n");
	printf(SPACE SPACE "It use a blob pattern to filter things\n\n");

	printf(SPACE "--lang: select the language backend (a compiler front-end)\n");
	printf(SPACE SPACE "Available backends:\n");

	for (size_t i = 0; i < BACKEND_REGISTER_L - 1; i++) print_backend(&BACKEND_REGISTER[i], &BACKEND_REGISTER[i + 1], i == 0, false);
	if (BACKEND_REGISTER_L > 0) print_backend(&BACKEND_REGISTER[BACKEND_REGISTER_L - 1], NULL, false, true);
	printf("\n\n");

	printf(SPACE "--data: configure what type of data should be on stdout\n");
	printf(SPACE SPACE "Syntax:\n");
	printf(SPACE SPACE SPACE "group.flag_1:flag_2:...:flag_n\n\n");
	printf(SPACE SPACE "Available groups:\n");
	printf(SPACE SPACE SPACE "frequency: data related to frequency\n");
	printf(SPACE SPACE SPACE "brute: data related to brute (single) values\n");
	printf(SPACE SPACE SPACE "bimap: data related to binary maps (WIP)\n\n");
	printf(SPACE SPACE "Available flags:\n");

	for (size_t i = 0; i < FLAGS_OPTS_L; i++) {
		printf(SPACE SPACE SPACE);
		printf("%-20.20s", FLAGS_OPTS[i].name);
		printf("%s\n", FLAGS_OPTS[i].description);
	}
	printf("\n\n");

	printf(SPACE "--profile: configure the tool's memory budget (also affects performance)\n");
	printf(SPACE SPACE "Accepts oen profile level\n");
	printf(SPACE SPACE "Available profiles:\n");

	for (size_t i = 0; i < PROFILE_OPT_L; i++) {
		printf(SPACE SPACE SPACE);
		printf("%-20.20s", PROFILE_OPT[i].name);
		printf("%s\n", PROFILE_OPT[i].description);
	}
	printf("\n\n");


	printf("-- Help menu end --\n");

	return false;
}

