#ifndef CODEREP_DATA_TYPES_DISTRIBUTIVE_H
#define CODEREP_DATA_TYPES_DISTRIBUTIVE_H

#include "coderep/data_types/string.h"

struct node {
	struct node *next;
	struct string *value;
	int count;
};

struct distributive {
	struct string *name;
	struct node **buckets;
	size_t buckets_count;
};

struct node *node_new(struct string **value);
void node_drop(struct node **self);

struct distributive *distributive_new(struct string **name, size_t bucket_count);

void distributive_push(struct distributive *self, struct string **value);

void distributive_drop(struct distributive **self);

#endif
