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
	println("(struct c_allocator) {");

	print("\t.bytes_allocated_n = ");
	print_size_t(self->bytes_allocated_n);
	println("");

	print("\t.bytes_reallocated_n = ");
	print_size_t(self->bytes_reallocated_n);
	println("");

	print("\t.allocation_n = ");
	print_size_t(self->allocation_n);
	println("");

	print("\t.realloc_n = ");
	print_size_t(self->realloc_n);
	println("");

	print("\t.free_n = ");
	print_size_t(self->free_n);
	println("\n}");
}

void c_allocator_drop(struct c_allocator **self)
{
	if (!self || !(*self)) return;

	free(*self);
	*self = NULL;
}

