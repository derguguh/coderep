#ifndef CODEREP_DATA_TYPES_ANY_H
#define CODEREP_DATA_TYPES_ANY_H

#include <stddef.h>

struct any {
	void *pointer;
	size_t size;
};

#endif
