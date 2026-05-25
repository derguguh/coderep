#include "coderep/backend.h"

#include "coderep/data_types/frequency.h"
#include "coderep/macros.h"

#include <stdlib.h>

struct data_array *data_array_alloc()
{
	struct brute **brutes = calloc(sizeof(struct brute *), DEFAULT_CAPACITY);
	struct list **lists = calloc(sizeof(struct list *), DEFAULT_CAPACITY);
	struct frequency **frequencies = calloc(sizeof(struct frequency *), DEFAULT_CAPACITY);

	struct data_array *self = malloc(sizeof(struct data_array));

	if (!self || !brutes || !lists || !frequencies) goto defer;

	self->brutes = brutes;
	self->lists = lists;
	self->frequencies = frequencies;

	self->brutes_l = 0;
	self->brutes_c = DEFAULT_CAPACITY;
	self->lists_l = 0;
	self->lists_c = DEFAULT_CAPACITY;
	self->frequencies_l = 0;
	self->frequencies_c = DEFAULT_CAPACITY;

	return self;
defer:
	free(self);
	free(brutes);
	free(lists);
	free(frequencies);

	MALLOC_ERR_EXIT("struct data_array (backend outputs)");
}

static inline void resize_generic_array(void ***data_o, size_t new_size)
{
	void **tmp = realloc(*data_o, sizeof(void *) * new_size);

	if (!tmp) goto defer;

	*data_o = tmp;

	return;
defer:
	PRINT_ERR("Couldn't reallocate memory to resize a struct data_array");
	exit(ERROR_CODE_MEMORY_REALLOCATION);
}

static inline void push_generic_array(void ***data_o, size_t *length_o, size_t *capacity_o, void **value)
{
	if (*length_o >= *capacity_o) {
		*capacity_o *= 2;
		resize_generic_array(data_o, *capacity_o);
	}

	(*data_o)[*length_o] = *value;
	(*length_o)++;
}

void data_array_push_brute(struct data_array *self, struct brute **brute)
{
	push_generic_array((void ***)&self->brutes, &self->brutes_l, &self->brutes_c, (void **)brute);
}

void data_array_push_list(struct data_array *self, struct list **list)
{
	push_generic_array((void ***)&self->lists, &self->lists_l, &self->lists_c, (void **)list);
}

void data_array_push_frequency(struct data_array *self, struct frequency **frequency)
{
	push_generic_array((void ***)&self->frequencies, &self->frequencies_l, &self->frequencies_c, (void **)frequency);
}

void data_array_print(struct data_array *self, struct data output_options)
{
	for (int i = 0; i < self->brutes_l; i++) {

		printf("%d: ", i);
		brute_print(self->brutes[i]);
	}
	for (int i = 0; i < self->lists_l; i++) list_print(self->lists[i]);
	for (int i = 0; i < self->frequencies_l; i++) frequency_print(self->frequencies[i]);
}

void data_array_drop(struct data_array **self)
{
	if (!self || !(*self)) return;

	for (int i = 0; i < (*self)->brutes_l; i++) brute_drop(&(*self)->brutes[i]);
	for (int i = 0; i < (*self)->lists_l; i++) list_drop(&(*self)->lists[i]);
	for (int i = 0; i < (*self)->frequencies_l; i++) frequency_drop(&(*self)->frequencies[i]);
	free((*self)->brutes);
	free((*self)->lists);
	free((*self)->frequencies);
	(*self)->brutes = NULL;
	(*self)->lists = NULL;
	(*self)->frequencies = NULL;

	free(*self);
}

