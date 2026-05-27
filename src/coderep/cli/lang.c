#include "coderep/cli/lang.h"

#include "coderep/backend.h"
#include "coderep/macros.h"

#include <string.h>

struct lang lang_create()
{
	struct lang lang = {
		._backend = NULL,
	};

	return lang;
}

int lang_parse(struct lang *self, const char **argv, int argc, int index)
{
	if (index >= argc) {

		PRINT_ERR("'--lang' option expects one flag");
		return index;
	}
	self->_backend = NULL;

	for (int i = 0; i < BACKEND_REGISTER_L; i++) {

		if (strcmp(BACKEND_REGISTER[i].identifier, argv[index]) == 0) self->_backend = &BACKEND_REGISTER[i];
	}

	if (self->_backend == NULL) {

		PRINTF_ERR("'%s' is not a valid language identifier", argv[index]);
		return index;
	} else return index + 1;
}

