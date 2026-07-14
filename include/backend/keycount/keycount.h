#ifndef BACKEND_KEYCOUNT_KEYCOUNT_H
#define BACKEND_KEYCOUNT_KEYCOUNT_H

#include "backend/backend.h"
#include "coderep/data_type/frequency.h"

struct keycount {
	struct frequency *digraph;
	struct frequency *single_keys;
	struct frequency *control;
	struct string *shift_template;
	struct string *key;
};

size_t keycount_request();

void keycount_init(struct backend_context *context);

void keycount_parse_file(struct backend_context *context, struct arena *ignore, struct file *file);

void keycount_deinit(struct backend_context *context);

#endif
