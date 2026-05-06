#include "back_ends/lua/token.h"

#include <stdio.h>

void token_print(const struct token self)
{
	printf("Token: { .type=%d, .id=%lu }\n", self.type, self.id);
}
