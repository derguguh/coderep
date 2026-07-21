#ifndef CODEREP_ALLOCATOR_C_ALLOCATOR_H
#define CODEREP_ALLOCATOR_C_ALLOCATOR_H

#include "coderep/allocator/allocator.h"

#include <stddef.h>

struct c_allocator {
	struct allocator abstract;
	size_t bytes_allocated_n;
	size_t bytes_reallocated_n;
	size_t allocation_n;
	size_t realloc_n;
	size_t free_n;
};

struct c_allocator *c_allocator_alloc();

void *c_allocator_allocate(struct c_allocator *self, size_t size);

void *c_allocator_reallocate(struct c_allocator *self, void *original_pointer, size_t new_size);

void  c_allocator_deallocate(struct c_allocator *self, void *pointer);

void c_allocator_print(struct c_allocator *self);

void c_allocator_drop(struct c_allocator **self);

#endif
