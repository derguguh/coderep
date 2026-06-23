#include "backend/c/lexer.h"

struct c_token c_lexer_next(struct c_lexer *self, struct file *file, struct string *scratch_buffer);

