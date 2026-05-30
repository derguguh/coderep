#ifndef CODEREP_ALLOCATOR_ARENA_H
#define CODEREP_ALLOCATOR_ARENA_H

#include "coderep/allocator/allocator.h"

#include <stddef.h>
#include <stdint.h>

#define DEFAULT_BLOCK_SIZE 8 * 1024 * 1024
#define PACKET_SIZE sizeof(size_t)
#define PACKET_ALIGN(size) (((size) + PACKET_SIZE - 1) & ~(PACKET_SIZE - 1))

struct block {
	uint8_t *mem;
	struct block *_next;
};

struct arena {
	struct allocator abstract;
	struct block *block;
	struct block *active_block;
	size_t block_size;
	size_t used;
	size_t current_block;
	size_t block_offset;
// Data collection
	size_t packets_allocated;
	size_t deallocation_tries;
};

struct arena *arena_alloc(size_t block_size);

void *arena_allocate(struct arena *self, size_t size);

void *arena_reallocate(struct arena *self, void *original_pointer, size_t new_size);

void  arena_deallocate(struct arena *self, void *pointer);

void  arena_clear(struct arena *self);

void  arena_drop(struct arena **self);

#endif
