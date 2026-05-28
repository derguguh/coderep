#include "backend/general/lexical_table.h"

#include "coderep/data_type/string.h"
#include "coderep/data_type/frequency.h"
#include "coderep/macros.h"

#include <stdlib.h>

struct general_lexical_table *general_lexical_table_alloc()
{
	struct string *tmp_str = string_from_cstr("text.words");
	struct frequency *word = frequency_alloc(&tmp_str, 20);

	tmp_str = string_from_cstr("text.numbers");
	struct frequency *number = frequency_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("text.characters");
	struct frequency *character = frequency_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("text.unknown");
	struct frequency *unknown = frequency_alloc(&tmp_str, 40);

	struct general_lexical_table *self = malloc(sizeof(struct general_lexical_table));
	if (!self) MALLOC_ERR_EXIT("struct lexer (for lua)");

	*self = (struct general_lexical_table) {
		.word = word,
		.number = number,
		.character = character,
		.unknown = unknown,
	};

	return self;
}

void general_lexical_table_append_to_data_array(struct general_lexical_table *self, struct data_array *data_array)
{
	data_array_push_frequency(data_array, &self->word);
	data_array_push_frequency(data_array, &self->number);
	data_array_push_frequency(data_array, &self->character);
	data_array_push_frequency(data_array, &self->unknown);

	self->word = NULL;
	self->number = NULL;
	self->character = NULL;
	self->unknown = NULL;
}

void general_lexical_table_drop(struct general_lexical_table **self)
{
	if (!self || !(*self)) return;

	frequency_drop(&(*self)->word);
	frequency_drop(&(*self)->number);
	frequency_drop(&(*self)->character);
	frequency_drop(&(*self)->unknown);

	string_drop(&(*self)->template_invalid_char);

	free(*self);
	*self = NULL;
}
