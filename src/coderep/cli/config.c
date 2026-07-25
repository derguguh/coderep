#include "coderep/cli/config.h"

#include "coderep/macros.h"

void config_input_expand(struct config *config)
{
	config->paths_c *= 2;
	struct string **tmp = config->allocator->reallocate(config->allocator->state, config->paths, sizeof(char *) * config->paths_c);

	if (!tmp) goto defer;

	config->paths = tmp;

	return;
defer:
	PRINT_ERR("Couldn't reallocate memory to handle all paths passed");
	exit(ERROR_CODE_MEMORY_REALLOCATION);
}
