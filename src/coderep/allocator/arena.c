#include "coderep/allocator/arena.h"

#include "coderep/allocator/allocator.h"
#include "coderep/consts.h"
#include "coderep/macros.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct arena *arena_alloc(size_t block_size)
{
	block_size = PACKET_ALIGN(block_size);

	struct allocator default_ = default_get_allocator();
	struct block *block = block_alloc(&default_, block_size);
	struct arena *self = default_.allocate(default_.state, sizeof(struct arena));
	if (!self) goto defer;

	*self = (struct arena) {
		.abstract = (struct allocator) {
			.state = self,
			.allocate = (allocate_t)arena_allocate,
			.reallocate = (reallocate_t)arena_reallocate,
			.deallocate = (deallocate_t)arena_deallocate,
		},
		.default_ = default_,
		.block = block,
		.active_block = block,
		.block_size = block_size,
		.current_block = 1,
		.block_count = 1,
	};

	return self;
defer:
	ALLOC_ERR_EXIT("struct arena (allocator)");
}

void *arena_allocate(struct arena *self, size_t size)
{
	size = PACKET_ALIGN(size);
	assert(size <= self->block_size);

	if (self->block_offset + size > self->block_size) {
		if (self->active_block->_next == NULL) {
			self->active_block->_next = block_alloc(&self->default_, self->block_size);
			self->block_count++;
		}

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
	self->block_offset = 0;

	self->active_block = self->block;
	self->current_block = 1;
}

void  arena_drop(struct arena **self)
{
	if (!self || !(*self)) return;

	(*self)->active_block = NULL;
	block_drop(&(*self)->default_, &(*self)->block);

	default_deallocate(NULL, *self);
	*self = NULL;
}

