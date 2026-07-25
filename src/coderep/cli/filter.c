#include "coderep/cli/filter.h"

bool filter_parse(struct config *config, struct argument_list *_list)
{
	string_clear(config->filter);
	string_append_cstr(config->filter, _list->arguments[0]);
	return true;
}

