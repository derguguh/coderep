#ifndef CODEREP_TOOL_COMMON_SKIPS_H
#define CODEREP_TOOL_COMMON_SKIPS_H

#include "coderep/tool/file.h"

typedef bool (*condition_fn)(char c);

size_t skip_while(struct file *file, condition_fn condition);

void skip_shebang(struct file *file);
size_t skip_blank(struct file *file);
size_t skip_whitespaces(struct file *file);

#endif
