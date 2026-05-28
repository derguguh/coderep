#ifndef BACKEND_GENERAL_LEXICAL_TABLE_H
#define BACKEND_GENERAL_LEXICAL_TABLE_H

#include "coderep/data_array.h"

struct general_lexical_table {
	struct frequency *word;
	struct frequency *number;
	struct frequency *character;
	struct frequency *unknown;
	struct string *template_invalid_char;
};

struct general_lexical_table *general_lexical_table_alloc();

void general_lexical_table_append_to_data_array(struct general_lexical_table *self, struct data_array *data_array);

void general_lexical_table_drop(struct general_lexical_table **self);

#endif
