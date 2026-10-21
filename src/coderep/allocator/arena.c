#include "coderep/allocator/arena.h"

#include "coderep/allocator/allocator.h"
#include "coderep/allocator/c_allocator.h"
#include "coderep/macros.h"
#include "coderep/tool/printer/printer.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct arena *arena_alloc(struct c_allocator *c_allocator, size_t block_sizeize)
{
	block_sizeize = PACKET_ALIGN(block_sizeize);

	struct block *block = block_alloc(&c_allocator->abstract, block_sizeize);
	struct arena *self = c_allocator_allocate(c_allocator, sizeof(struct arena));
	if (!self) goto defer;

	*self = (struct arena) {
		.abstract = (struct allocator) {
			.state = self,
			.allocate = (allocate_fn)arena_allocate,
			.reallocate = (reallocate_fn)arena_reallocate,
			.deallocate = (deallocate_fn)arena_deallocate,
		},
		.allocator = c_allocator,
		.block = block,
		.active_block = block,
		.block_size = block_sizeize,
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
			self->active_block->_next = block_alloc(&self->allocator->abstract, self->block_size);
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
	print_err("struct arena don't support realloc. If you need, consider default allocator");
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
	block_drop(&(*self)->allocator->abstract, &(*self)->block);

	c_allocator_deallocate((*self)->allocator, *self);
	*self = NULL;
}

