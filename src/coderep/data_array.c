#include "coderep/data_array.h"

#include "coderep/data_type/frequency.h"
#include "coderep/data_type/output_mode.h"
#include "coderep/macros.h"

#include <stdlib.h>
#include <string.h>

const size_t DATA_ARRAY_DEFAULT_CAPACITY = 20;

struct generic_array {
	void **ptr;
	size_t length;
	size_t capacity;
};

struct data_array *data_array_alloc()
{
	struct brute **brutes = calloc(sizeof(struct brute *), DATA_ARRAY_DEFAULT_CAPACITY);
	struct frequency **frequencies = calloc(sizeof(struct frequency *), DATA_ARRAY_DEFAULT_CAPACITY);

	struct data_array *self = malloc(sizeof(struct data_array));

	if (!self || !brutes || !frequencies) goto defer;

	*self = (struct data_array) {
		.brutes = brutes,
		.frequencies = frequencies,

		.brutes_c = DATA_ARRAY_DEFAULT_CAPACITY,
		.frequencies_c = DATA_ARRAY_DEFAULT_CAPACITY,
	};

	return self;
defer:
	free(self);
	free(brutes);
	free(frequencies);

	MALLOC_ERR_EXIT("struct data_array (backend outputs)");
}

static inline void resize_generic_expand(struct generic_array *array)
{
	array->capacity *= 2;
	void **tmp = realloc(array->ptr, sizeof(void *) * array->capacity);

	if (!tmp) goto defer;

	array->ptr = tmp;

	return;
defer:
	PRINT_ERR("Couldn't reallocate memory to resize a struct data_array");
	exit(ERROR_CODE_MEMORY_REALLOCATION);
}

static inline void push_generic_array(struct generic_array *array, void **value)
{
	if (array->length >= array->capacity) resize_generic_expand(array);

	array->ptr[array->length] = *value;
	array->length++;

	*value = NULL;
}

void data_array_push_brute(struct data_array *self, struct brute **brute)
{
	struct generic_array array = {
		.ptr = (void **)self->brutes,
		.length = self->brutes_l,
		.capacity = self->brutes_c,
	};
	push_generic_array(&array, (void **)brute);

	self->brutes = (struct brute **)array.ptr;
	self->brutes_l = array.length;
	self->brutes_c = array.capacity;
}

void data_array_push_frequency(struct data_array *self, struct frequency **frequency)
{
	struct generic_array array = {
		.ptr = (void **)self->frequencies,
		.length = self->frequencies_l,
		.capacity = self->frequencies_c,
	};
	push_generic_array(&array, (void **)frequency);

	self->frequencies = (struct frequency **)array.ptr;
	self->frequencies_l = array.length;
	self->frequencies_c = array.capacity;
}

void data_array_print(struct data_array *self, struct data *output_options)
{
	if (output_options->frequency != OUTPUT_MODE_NONE) {
		for (size_t i = 0; i < self->frequencies_l; i++) frequency_print(self->frequencies[i], output_options->frequency);
	}
	if (output_options->brute != OUTPUT_MODE_NONE) {
		for (size_t i = 0; i < self->brutes_l; i++) brute_print(self->brutes[i], output_options->brute);
	}
}

void data_array_drop(struct data_array **self)
{
	if (!self || !(*self)) return;

	for (size_t i = 0; i < (*self)->brutes_l; i++) brute_drop(&(*self)->brutes[i]);
	for (size_t i = 0; i < (*self)->frequencies_l; i++) frequency_drop(&(*self)->frequencies[i]);
	free((*self)->brutes);
	free((*self)->frequencies);
	(*self)->brutes = NULL;
	(*self)->frequencies = NULL;

	free(*self);
}

