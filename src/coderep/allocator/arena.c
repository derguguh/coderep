#include "coderep/allocator/arena.h"

#include "coderep/allocator/allocator.h"
#include "coderep/consts.h"
#include "coderep/macros.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

struct block *block_alloc(size_t block_size)
{
	struct block *self = default_allocate(NULL, sizeof(struct block));
	uint8_t *mem = default_allocate(NULL, block_size);
	if (!self || !mem) goto defer;

	*self = (struct block) {
		.mem = mem,
		._next = NULL,
	};

	return self;
defer:
	MALLOC_ERR_EXIT("struct block");
}

void block_drop(struct block **self)
{
	if ((*self)->_next != NULL) block_drop(&(*self)->_next);

	default_deallocate(NULL, (*self)->mem);
	(*self)->mem = NULL;

	default_deallocate(NULL, *self);
	*self = NULL;
}

struct arena *arena_alloc(size_t block_size)
{
	block_size = PACKET_ALIGN(block_size);

	struct block *block = block_alloc(block_size);
	struct arena *self = default_allocate(NULL, sizeof(struct arena));
	if (!self) goto defer;

	*self = (struct arena) {
		.block = block,
		.block_size = block_size,
		.current_block = 1,
	};

	return self;
defer:
	MALLOC_ERR_EXIT("struct arena (allocator)");
}

void *arena_allocate(struct arena *self, size_t size)
{
	struct block *block = self->block;
	for (int i = 1; i < self->current_block; i++) block = block->_next;

	size = PACKET_ALIGN(size);
	self->used += size;

	if (self->current_block * self->block_size < self->used) {
		if (block->_next == NULL) block->_next = block_alloc(self->block_size);

		block = block->_next;
		self->current_block++;
		self->block_offset = 0;
	}

	void *pointer = block->mem + self->block_offset;
	self->block_offset += size;

	self->packets_allocated += size >> 3;

	return pointer;
}

void *arena_reallocate(struct arena *self, void *original_pointer, size_t new_size)
{
	PRINT_ERR("struct arena don't support realloc. If you need, consider default allocator");
	exit(ERROR_CODE_UNSUPPORTED);
}

void  arena_deallocate(struct arena *self, void *pointer)
{
	self->deallocation_tries++;
}

void  arena_clean(struct arena *self)
{
	self->used = 0;
	self->current_block = 0;
}

struct allocator arena_get_allocator(struct arena *self)
{
	return (struct allocator) {
		.state = self,
		.allocate = (allocate_t)arena_allocate,
		.reallocate = (reallocate_t)arena_reallocate,
		.deallocate = (deallocate_t)arena_deallocate,
	};
}

void  arena_drop(struct arena **self)
{
	if (!self || !(*self)) return;

	block_drop(&(*self)->block);

	default_deallocate(NULL, *self);
	*self = NULL;
}

