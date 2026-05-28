#include "backend/lua/lexical_table.h"

#include "coderep/data_type/string.h"
#include "coderep/data_type/frequency.h"
#include "coderep/macros.h"

#include <stdlib.h>

struct lua_lexical_table *lua_lexical_table_alloc()
{
	struct string *tmp_str = string_from_cstr("lexer.symbols.block_strings");
	struct frequency *block_string = frequency_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.symbols.strings");
	struct frequency *string = frequency_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.symbols.identifiers");
	struct frequency *identifiers = frequency_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.symbols.numbers");
	struct frequency *number = frequency_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.symbols.keywords");
	struct frequency *keyword = frequency_alloc(&tmp_str, 20);

	tmp_str = string_from_cstr("lexer.symbols.characters");
	struct frequency *character = frequency_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.invalid.table");
	struct frequency *invalid = frequency_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.invalid.what");
	struct frequency *what_invalid = frequency_alloc(&tmp_str, 40);

	struct string *template_invalid_number = string_from_cstr("Invalid: Number");
	struct string *template_invalid_string = string_from_cstr("Invalid: String");
	struct string *template_invalid_char = string_from_cstr("Invalid: Character");

	struct lua_lexical_table *self = malloc(sizeof(struct lua_lexical_table));
	if (!self) MALLOC_ERR_EXIT("struct lexer (for lua)");

	*self = (struct lua_lexical_table) {
		.block_string = block_string,
		.string = string,
		.identifier = identifiers,
		.number = number,
		.keyword = keyword,
		.character = character,
		.invalid = invalid,
		.what_invalid = what_invalid,

		.template_invalid_number = template_invalid_number,
		.template_invalid_string = template_invalid_string,
		.template_invalid_char = template_invalid_char,
	};

	return self;
}

void lua_lexical_table_append_to_data_array(struct lua_lexical_table *self, struct data_array *data_array)
{
	data_array_push_frequency(data_array, &self->block_string);
	data_array_push_frequency(data_array, &self->string);
	data_array_push_frequency(data_array, &self->identifier);
	data_array_push_frequency(data_array, &self->number);
	data_array_push_frequency(data_array, &self->keyword);
	data_array_push_frequency(data_array, &self->character);
	data_array_push_frequency(data_array, &self->invalid);
	data_array_push_frequency(data_array, &self->what_invalid);

	self->block_string = NULL;
	self->string = NULL;
	self->identifier = NULL;
	self->number = NULL;
	self->keyword = NULL;
	self->character = NULL;
	self->invalid = NULL;
	self->what_invalid = NULL;
}

void lua_lexical_table_drop(struct lua_lexical_table **self)
{
	if (!self || !(*self)) return;

	frequency_drop(&(*self)->block_string);
	frequency_drop(&(*self)->string);
	frequency_drop(&(*self)->identifier);
	frequency_drop(&(*self)->number);
	frequency_drop(&(*self)->keyword);
	frequency_drop(&(*self)->character);
	frequency_drop(&(*self)->invalid);
	frequency_drop(&(*self)->what_invalid);

	string_drop(&(*self)->template_invalid_number);
	string_drop(&(*self)->template_invalid_string);
	string_drop(&(*self)->template_invalid_char);

	free(*self);
	*self = NULL;
}
