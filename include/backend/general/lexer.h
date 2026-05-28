#ifndef BACKEND_GENERAL_LEXER_H
#define BACKEND_GENERAL_LEXER_H

#include "backend/general/lexical_table.h"
#include "backend/general/token.h"
#include "coderep/tool/file.h"

#include <stddef.h>

enum general_token_type general_lexer_next(struct file *file, struct general_lexical_table *table);

#endif
