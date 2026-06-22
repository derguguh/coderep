#include "coderep/allocator/block.h"

#include "coderep/macros.h"

#include <string.h>

struct block *block_alloc(struct allocator *allocator, size_t block_size)
{
	struct block *self = allocator->allocate(allocator->state, sizeof(struct block));
	uint8_t *mem = allocator->allocate(allocator->state, block_size);
	if (!self || !mem) goto defer;

	memset(mem, 0, block_size);

	*self = (struct block) {
		._next = NULL,
		.mem = mem,
	};

	return self;
defer:
	MALLOC_ERR_EXIT("struct block");
}

void block_drop(struct allocator *allocator, struct block **self)
{
	if (!self || !(*self)) return;
	if ((*self)->_next != NULL) block_drop(allocator, &(*self)->_next);

	allocator->deallocate(allocator->state, (*self)->mem);
	(*self)->mem = NULL;

	allocator->deallocate(allocator->state, *self);
	*self = NULL;
}

