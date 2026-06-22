#ifndef CODEREP_ALLOCATOR_BLOCK_H
#define CODEREP_ALLOCATOR_BLOCK_H

#include "coderep/allocator/allocator.h"

#include <stdint.h>

#define DEFAULT_BLOCK_SIZE 8 * 1024 * 1024

struct block {
	struct block *_next;
	uint8_t *mem;
};

struct block *block_alloc(struct allocator *allocator, size_t block_size);

void block_drop(struct allocator *allocator, struct block **self);

#endif
