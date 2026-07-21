#ifndef CODEREP_ALLOCATOR_ALLOCATOR_H
#define CODEREP_ALLOCATOR_ALLOCATOR_H

#include <stddef.h>
#include <stdlib.h>

typedef void *(*allocate_fn)(void *state, size_t size);
typedef void *(*reallocate_fn)(void *state, void *original_pointer, size_t new_size);
typedef void  (*deallocate_fn)(void *state, void *pointer);

struct allocator {
	void *state;
	allocate_fn allocate;
	reallocate_fn reallocate;
	deallocate_fn deallocate;
};

static inline void *default_allocate(void *ignore, size_t size)
{
	return malloc(size);
}

static inline void *default_reallocate(void *ignore, void *original_pointer, size_t new_size)
{
	return realloc(original_pointer, new_size);
}

static inline void  default_deallocate(void *ignore, void *pointer)
{
	free(pointer);
}

static inline const struct allocator default_get_allocator()
{
	struct allocator allocator = {
		.state = NULL,
		.allocate = default_allocate,
		.reallocate = default_reallocate,
		.deallocate = default_deallocate,
	};

	return allocator;
}

#endif
