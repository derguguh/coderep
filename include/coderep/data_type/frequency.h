#ifndef CODEREP_DATA_TYPE_FREQUENCY_H
#define CODEREP_DATA_TYPE_FREQUENCY_H

#include "coderep/data_type/output_mode.h"
#include "coderep/data_type/string.h"
#include <stdbool.h>

enum frequency_type {
	FREQUENCY_TYPE_STRING,
	FREQUENCY_TYPE_DOUBLE,
	FREQUENCY_TYPE_SSIZE_T,
	FREQUENCY_TYPE_SIZE_T,
};

struct frequency;

extern const size_t FREQUENCY_DEFAULT_SIZE;

/*
 * Future idea: frequency for other types besides string, with type in structure allocation
 *
 * struct frequency *frequency_alloc(struct allocator *allocator, enum frequency_type type, size_t initial_capacity);
 *
 */
struct frequency *frequency_alloc(struct allocator *allocator, size_t initial_capacity);

bool frequency_has(struct frequency *self, const struct string *value);

bool frequency_increment(struct frequency *self, const struct string *value);

bool frequency_intern(struct frequency *self, const struct string *value);

void frequency_push_string(struct frequency *self, struct string **value);

struct stats frequency_calculate_stats(const struct frequency *self);

void frequency_print(const struct frequency *self, enum output_mode output_mode, bool do_color);

void frequency_drop(struct frequency **self);

#endif
