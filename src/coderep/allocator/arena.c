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
		.abstract = (struct allocator) {
			.state = self,
			.allocate = (allocate_t)arena_allocate,
			.reallocate = (reallocate_t)arena_reallocate,
			.deallocate = (deallocate_t)arena_deallocate,
		},
		.block = block,
		.active_block = block,
		.block_size = block_size,
		.current_block = 1,
	};

	return self;
defer:
	MALLOC_ERR_EXIT("struct arena (allocator)");
}

void *arena_allocate(struct arena *self, size_t size)
{
	size = PACKET_ALIGN(size);
	self->used += size;

	if (self->current_block * self->block_size < self->used) {
		if (self->active_block->_next == NULL) self->active_block->_next = block_alloc(self->block_size);

		self->active_block = self->active_block->_next;
		self->current_block++;
		self->block_offset = 0;
	}

	void *pointer = self->active_block->mem + self->block_offset;
	self->block_offset += size;

	self->packets_allocated += size / PACKET_SIZE;

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

void  arena_clear(struct arena *self)
{
	self->used = 0;
	self->current_block = 0;
}

void  arena_drop(struct arena **self)
{
	if (!self || !(*self)) return;

	printf("Was need %lu new blocks\n", (*self)->current_block);
	printf("Packets used: %lu\n", (*self)->packets_allocated);

	(*self)->active_block = NULL;
	block_drop(&(*self)->block);

	default_deallocate(NULL, *self);
	*self = NULL;
}

