#include "backend/general/general.h"

#include "backend/general/lexer.h"
#include "backend/general/lexical_table.h"
#include "backend/general/token.h"
#include "coderep/cli/input.h"
#include "coderep/data_array.h"
#include "coderep/tool/file.h"
#include "coderep/consts.h"

struct data_array *general_parse(const struct input *input)
{
	struct data_array *data_array = data_array_alloc();

	struct general_lexical_table *lexical_table = general_lexical_table_alloc();

	struct file *file = file_alloc();

	for (size_t i = 0; i < input->files_l; i++) {

		file_open(file, input->files[i]);
		if (file->buffer->_file == NULL) continue;
		printf(ANSI_ESC ANSI_GREEN "[File "
				ANSI_ESC ANSI_RESET "%lu/%lu"
				ANSI_ESC ANSI_GREEN ":"
				ANSI_ESC ANSI_RESET "%s" ANSI_ESC ANSI_GREEN
				ANSI_ESC ANSI_GREEN "]"
				ANSI_ESC ANSI_RESET " \n", i, input->files_l, input->files[i]);

		enum general_token_type token = general_lexer_next(file, lexical_table);
		while (token != TOKEN_TYPE_EOF) token = general_lexer_next(file, lexical_table);
	}

	file_drop(&file);

	general_lexical_table_append_to_data_array(lexical_table, data_array);

	general_lexical_table_drop(&lexical_table);

	return data_array;
}

