#ifndef CODEREP_BACK_END_LUA_LEXER_H
#define CODEREP_BACK_END_LUA_LEXER_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define BUFFER_SIZE 4096

struct lexer {
	uint8_t buffer[BUFFER_SIZE];
	size_t buffer_cursor;
	size_t buffer_lenth;
	FILE *file;
};

struct lexer *lexer_new(char);

void lexer_drop(struct lexer **self);

#endif
