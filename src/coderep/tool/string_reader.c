#include "coderep/tool/string_reader.h"

#include "coderep/data_type/string.h"
#include "coderep/tool/common_readers.h"
#include <assert.h>

bool string_handler_read_char(struct string_handler *self, struct file *file, struct string *scratch_buffer, struct arena *temporary)
{
	switch (file->buffer->char_) {
	case EOF:
		if (!self->can_end_at_eof) file_print_error(file, "Strings need to be finalized with their own end sequence, but EOF (End of File) was founded", 1);
		return false;
	break;
	case '\n':
		if (!self->accept_new_line) {
			file_print_error(file, "This string don't accept new lines", 1);
			return false;
		}
	default:
		if (self->_escape_handler != NULL && file->buffer->char_ == self->escape_char) {
			if (!self->_escape_handler(file, scratch_buffer, temporary)) return false;
		} else push_and_advance(file, scratch_buffer);
	break;
	}
	return true;
}

bool string_handler_check_end_sequence(struct string_handler *self, struct file *file, struct string *scratch_buffer)
{
	for (size_t i = 1; i <= self->end_sequence->length; i++) {
		if (scratch_buffer->chars[scratch_buffer->length - i] != self->end_sequence->chars[self->end_sequence->length - i]) return false;
	}
	return true;
}

ssize_t string_handler_read(struct string_handler *self, struct file *file, struct string *scratch_buffer, struct arena *temporary)
{
	assert(self->end_sequence->length > 0 && "The end sequence must have at least one character (UB will be caused)");

	char last_of_sequence = self->end_sequence->chars[self->end_sequence->length - 1];

	ssize_t amount_read = 0;
	bool had_errors = false;
	while(true) {
		if (file->buffer->char_ == last_of_sequence) {
			push_and_advance(file, scratch_buffer);
			if (string_handler_check_end_sequence(self, file, scratch_buffer)) break;
		} else if (!string_handler_read_char(self, file, scratch_buffer, temporary)) {
			had_errors = true;
			break;
		}
		amount_read++;
	}

	if (had_errors) return -1;
	if (!self->keep_end_sequence) scratch_buffer->length -= self->end_sequence->length;
	return amount_read;
}

