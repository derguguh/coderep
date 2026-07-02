#include "backend/c/c.h"

#include "backend/c/lexer.h"

size_t c_request()
{
	return sizeof(struct c_lexer);
}

void c_init(struct backend_context *data)
{

}

void c_parse_file(struct backend_context *data, struct arena *file_allocator, struct file *file)
{

}

void c_deinit(struct backend_context *data)
{

}

