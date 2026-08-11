#include "coderep/tool/common_skips.h"

#include <ctype.h>

size_t skip_while(struct file *file, condition_fn condition)
{
	size_t amount = 0;
	while (condition(file->buffer->char_) && file->buffer->char_ != EOF) {
		file_advance(file);
		amount++;
	}
	return amount;
}

void skip_shebang(struct file *file)
{
	if (file->buffer->char_ == '#' && file_buffer_peek_n(file->buffer, 1) == '!') {
		while (file->buffer->char_ != '\n' && file->buffer->char_ != EOF) file_advance(file);
	}
}

size_t skip_blank(struct file *file)
{
	size_t amount = 0;
	while (isblank(file->buffer->char_) && file->buffer->char_ != EOF) {
		file_advance(file);
		amount++;
	}
	return amount;
}

size_t skip_whitespaces(struct file *file)
{
	size_t amount = 0;
	while (isspace(file->buffer->char_) && file->buffer->char_ != EOF) {
		file_advance(file);
		amount++;
	}
	return amount;
}

