#include "coderep/data_array.h"

#include "coderep/macros.h"

#include <stdlib.h>

const size_t DATA_ARRAY_DEFAULT_CAPACITY = 20;

struct data_array *data_array_alloc()
{
	struct brute **brutes = calloc(sizeof(struct brute *), DEFAULT_CAPACITY);
	struct frequency **frequencies = calloc(sizeof(struct frequency *), DEFAULT_CAPACITY);

	struct data_array *self = malloc(sizeof(struct data_array));

	if (!self || !brutes || !frequencies) goto defer;

	*self = (struct data_array) {
		.brutes = brutes,
		.frequencies = frequencies,

		.brutes_c = DEFAULT_CAPACITY,
		.frequencies_c = DEFAULT_CAPACITY,
	};

	return self;
defer:
	free(self);
	free(brutes);
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

void data_array_push_frequency(struct data_array *self, struct frequency **frequency)
{
	push_generic_array((void ***)&self->frequencies, &self->frequencies_l, &self->frequencies_c, (void **)frequency);
}

void data_array_print(struct data_array *self, struct data output_options)
{
	for (int i = 0; i < self->brutes_l; i++) brute_print(self->brutes[i], output_options.brute);
	for (int i = 0; i < self->frequencies_l; i++) frequency_print(self->frequencies[i], output_options.frequency);
}

void data_array_drop(struct data_array **self)
{
	if (!self || !(*self)) return;

	for (int i = 0; i < (*self)->brutes_l; i++) brute_drop(&(*self)->brutes[i]);
	for (int i = 0; i < (*self)->frequencies_l; i++) frequency_drop(&(*self)->frequencies[i]);
	free((*self)->brutes);
	free((*self)->lists);
	free((*self)->frequencies);
	(*self)->brutes = NULL;
	(*self)->lists = NULL;
	(*self)->frequencies = NULL;

	free(*self);
}

