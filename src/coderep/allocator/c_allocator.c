#include "coderep/allocator/c_allocator.h"
#include "coderep/macros.h"

struct c_allocator *c_allocator_alloc()
{
	struct c_allocator *self = malloc(sizeof(struct c_allocator));
	if (!self) goto defer;
	*self = (struct c_allocator) {
		.abstract = (struct allocator) {
			.state = self,
			.allocate = (allocate_fn)c_allocator_allocate,
			.reallocate = (reallocate_fn)c_allocator_reallocate,
			.deallocate = (deallocate_fn)c_allocator_deallocate,
		}
	};

	return self;
defer:
	ALLOC_ERR_EXIT("struct c_allocator");
}

void *c_allocator_allocate(struct c_allocator *self, size_t size)
{
	self->allocation_n++;
	self->bytes_allocated_n += size;
	return malloc(size);
}

void *c_allocator_reallocate(struct c_allocator *self, void *original_pointer, size_t new_size)
{
	self->realloc_n++;
	self->bytes_reallocated_n += new_size;
	return realloc(original_pointer, new_size);
}

void  c_allocator_deallocate(struct c_allocator *self, void *pointer)
{
	self->free_n++;
	free(pointer);
}

void c_allocator_print(struct c_allocator *self)
{
	printf("(struct c_allocator) {\n");
	printf("\t.bytes_allocated_n = %zu,\n", self->bytes_allocated_n);
	printf("\t.bytes_reallocated_n = %zu,\n", self->bytes_reallocated_n);
	printf("\t.allocation_n = %zu,\n", self->allocation_n);
	printf("\t.realloc_n = %zu,\n", self->realloc_n);
	printf("\t.free_n = %zu,\n", self->free_n);
	printf("}\n");
}

void c_allocator_drop(struct c_allocator **self)
{
	if (!self || !(*self)) return;

	free(*self);
	*self = NULL;
}

