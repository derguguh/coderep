#ifndef BACKEND_LUA_LEXER_H
#define BACKEND_LUA_LEXER_H

#include "backend/lua/lexical_table.h"
#include "backend/lua/token.h"
#include "coderep/tool/file.h"

#include <stddef.h>

enum lua_token_type lua_lexer_identifier_word(struct string *word);

struct lua_token lua_lexer_next(struct file *file, struct lua_lexical_table *table);

void lua_lexer_skip_shebang(struct file *file);

#endif
