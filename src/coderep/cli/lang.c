#include "coderep/cli/lang.h"

#include "coderep/tool/printer/printer.h"

#include <string.h>

bool lang_parse(struct config *config, struct argument_list *_list)
{
	for (size_t i = 0; i < BACKEND_REGISTER_L; i++) {
		if (strcmp(BACKEND_REGISTER[i].identifier, _list->arguments[0]) == 0) config->_backend = &BACKEND_REGISTER[i];
	}

	if (config->_backend == NULL) {
		printf_err("'%s' is not a valid language identifier", _list->arguments[0]);
		return false;
	} else return true;
}

