#include "coderep/cli/source.h"

#include "coderep/tool/file_buffer.h"

#include <stdio.h>
#include <string.h>

void read_file(struct string *buffer, struct file_buffer *file)
{
	while (file->char_ != '\n' && file->char_ != '\0' && file->char_ != EOF) {
		string_push(buffer, file->char_);
		file_buffer_advance(file);
	}
	file_buffer_advance(file);
}

bool source_parse(struct config *config, struct argument_list *_list)
{
	struct string *scratch_buffer = string_alloc(config->allocator, 128);
	struct file_buffer *file = file_buffer_alloc(config->allocator);

	if (strcmp("-", _list->arguments[0]) == 0) {
		FILE *stdin_ = stdin;
		file_buffer_open_stream(file, &stdin_);
	} else file_buffer_open(file, _list->arguments[0]);

	if (file->_file == NULL) return false;

	while (file->char_ != EOF) {
		read_file(scratch_buffer, file);
		if (scratch_buffer->length == 0) {
			string_clear(scratch_buffer);
			continue;
		}
		
		if (config->paths_l >= config->paths_c) config_input_expand(config);
		config->paths[config->paths_l] = string_clone(config->allocator, scratch_buffer, true);
		string_clear(scratch_buffer);

		config->paths_l++;
	}

	file_buffer_drop(&file);
	string_drop(&scratch_buffer);

	return true;
}

