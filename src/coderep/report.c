#include "coderep/report.h"

#include "coderep/data_type/frequency.h"
#include "coderep/data_type/output_mode.h"
#include "coderep/macros.h"

#include <stdlib.h>
#include <string.h>

const size_t _DEFAULT_CAPACITY = 20;

struct generic_array {
	void **ptr;
	size_t length;
	size_t capacity;
};

struct report *report_alloc()
{
	struct brute **brutes = calloc(sizeof(struct brute *), _DEFAULT_CAPACITY);
	struct frequency **frequencies = calloc(sizeof(struct frequency *), _DEFAULT_CAPACITY);

	struct report *self = malloc(sizeof(struct report));

	if (!self || !brutes || !frequencies) goto defer;

	*self = (struct report) {
		.brutes = brutes,
		.frequencies = frequencies,

		.brutes_c = _DEFAULT_CAPACITY,
		.frequencies_c = _DEFAULT_CAPACITY,
	};

	return self;
defer:
	free(self);
	free(brutes);
	free(frequencies);

	MALLOC_ERR_EXIT("struct report (backend outputs)");
}

static inline void resize_generic_expand(struct generic_array *array)
{
	array->capacity *= 2;
	void **tmp = realloc(array->ptr, sizeof(void *) * array->capacity);

	if (!tmp) goto defer;

	array->ptr = tmp;

	return;
defer:
	PRINT_ERR("Couldn't reallocate memory to resize a struct report");
	exit(ERROR_CODE_MEMORY_REALLOCATION);
}

static inline void push_generic_array(struct generic_array *array, void **value)
{
	if (array->length >= array->capacity) resize_generic_expand(array);

	array->ptr[array->length] = *value;
	array->length++;

	*value = NULL;
}

void report_push_brute(struct report *self, struct brute **brute)
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

void report_push_frequency(struct report *self, struct frequency **frequency)
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

void report_print(struct report *self, struct data *output_options)
{
	printf("\n");
	if (output_options->frequency != OUTPUT_MODE_NONE) {
		for (size_t i = 0; i < self->frequencies_l; i++) frequency_print(self->frequencies[i], output_options->frequency);
		printf("\n");
	}
	if (output_options->brute != OUTPUT_MODE_NONE) {
		for (size_t i = 0; i < self->brutes_l; i++) brute_print(self->brutes[i], output_options->brute);
		printf("\n");
	}
}

void report_drop(struct report **self)
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

