#include "coderep/cli/input.h"

#include "coderep/cli/option.h"
#include "coderep/data_type/string.h"

#include <stdlib.h>
#include <glob.h>

bool input_parse(struct config *config, struct argument_list *_list)
{
	for (size_t i = 0; i < _list->arguments_l; i++) {
		if (config->paths_l >= config->paths_c) config_input_expand(config);
		config->paths[config->paths_l] = string_from_cstr(config->allocator, _list->arguments[i], true);

		config->paths_l++;
	}

	return true;
}

