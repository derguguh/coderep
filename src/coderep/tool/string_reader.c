#include "coderep/tool/string_reader.h"

#include "coderep/data_type/string.h"
#include "coderep/tool/common_readers.h"

bool string_handler_read_char(struct string_handler *self, struct file *file, struct string *buffer, struct arena *tmp)
{
	switch (file->buffer->char_) {
	case EOF:
		file_print_error(file, "Strings need to be finalized with their own end sequence, but EOF (End of File) was founded", 1);
		return false;
	break;
	case '\n':
		if (!self->accept_new_line) {
			file_print_error(file, "This string don't accept new lines", 1);
			return false;
		}
	default:
		if (self->_escape_handler != NULL && file->buffer->char_ == self->escape_char) {
			if (!self->_escape_handler(file, buffer, tmp)) return false;
		} else {
			push_and_advance(file, buffer);
		}
	break;
	}
	return true;
}

bool string_handler_skip_char(struct string_handler *self, struct file *file, struct arena *tmp)
{
	switch (file->buffer->char_) {
	case EOF:
		file_print_error(file, "Strings need to be finalized with their own end sequence, but EOF (End of File) was founded", 1);
		return false;
	break;
	case '\n':
		if (!self->accept_new_line) {
			file_print_error(file, "This string don't accept new lines", 1);
			return false;
		}
	default:
		if (self->_escape_handler != NULL && file->buffer->char_ == self->escape_char) {
			if (!self->_escape_handler(file, NULL, tmp)) return false;
		} else {
			file_advance(file);
		}
	break;
	}
	return true;
}

bool string_handler_check_substring(struct string_handler *self, struct file *file, struct string *_buffer, struct string *tmp)
{
	return false;
}

// Now this probably will not work...  but is looking way better
bool string_handler_check_end_sequence(struct string_handler *self, struct file *file, struct string *_buffer, struct string *tmp)
{
	string_clear(tmp);
	size_t founded_first = 0;
	for (size_t i = 0; i < self->end_sequence->length; i++) {
		if (string_get(self->end_sequence, i) == file->buffer->char_) push_and_advance(file, tmp);
		else {
			if (_buffer != NULL) string_append(_buffer, tmp);
			if (string_get(self->end_sequence, 0) == file->buffer->char_ && founded_first == 0) founded_first = i; 
		}
	}
	if (tmp->length == self->end_sequence->length) return true;
	return true;
}

ssize_t string_handler_read(struct string_handler *self, struct file *file, struct string *_buffer, struct arena *tmp)
{
	struct string *end_sequence = string_alloc(&tmp->abstract, self->end_sequence->capacity * 2);
	ssize_t amount_read = 0;

	while (true) {
		amount_read++;
		string_handler_check_end_sequence(self, file, _buffer, end_sequence);

		if (_buffer == NULL) string_handler_skip_char(self, file, tmp);
		else string_handler_read_char(self, file, _buffer, tmp);
	}

	return amount_read;
}

