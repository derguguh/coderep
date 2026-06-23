#include "backend/c/c.h"

#include "backend/c/lexer.h"

size_t c_request()
{
	return sizeof(struct c_lexer);
}

void c_init(struct backend_data *data)
{

}

void c_parse_file(struct backend_data *data, struct arena *file_allocator, struct file *file)
{

}

void c_deinit(struct backend_data *data)
{

}

