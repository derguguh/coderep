#ifndef CODEREP_BACKEND_PARSING_H
#define CODEREP_BACKEND_PARSING_H

#include "coderep/allocator/c_allocator.h"
#include "coderep/cli/config.h"

void parse_backend(const struct config *config, struct c_allocator *allocator);

#endif
