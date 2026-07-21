#include "coderep/cli/input.h"

#include "coderep/cli/option.h"
#include "coderep/data_type/string.h"
#include "coderep/macros.h"

#include <stdlib.h>

void input_expand(struct config *config)
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

bool input_parse(struct config *config, struct argument_list *_list)
{
	for (size_t i = 0; i < _list->arguments_l; i++) {
		if (config->paths_l >= config->paths_c) input_expand(config);
		config->paths[config->paths_l] = string_from_cstr(config->allocator, _list->arguments[i]);

		config->paths_l++;
	}

	return true;
}

