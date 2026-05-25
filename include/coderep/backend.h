#ifndef CODEREP_BACKEND_H
#define CODEREP_BACKEND_H

#include "coderep/cli/data.h"
#include "coderep/data_types/string.h"
#include "coderep/data_types/brute.h"
#include "coderep/data_types/list.h"
#include "coderep/data_types/frequency.h"

#include <stddef.h>

typedef struct data_array *(*parse_files_t)();

struct backend {
	struct string *name;
	parse_files_t parsing_finction;
};

struct data_array {
	struct brute **brutes;
	size_t brutes_l;
	size_t brutes_c;
	struct list **lists;
	size_t lists_l;
	size_t lists_c;
	struct frequency **frequencies;
	size_t frequencies_l;
	size_t frequencies_c;
};

const static size_t DEFAULT_CAPACITY = 20;

struct data_array *data_array_alloc();

void data_array_push_brute(struct data_array *self, struct brute **brute);
void data_array_push_list(struct data_array *self, struct list **list);
void data_array_push_frequency(struct data_array *self, struct frequency **frequency);

void data_array_print(struct data_array *self, struct data output_options);

void data_array_drop(struct data_array **self);

#endif
