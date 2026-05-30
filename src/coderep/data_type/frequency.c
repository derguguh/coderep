#include "coderep/data_type/frequency.h"

#include "coderep/allocator/allocator.h"
#include "coderep/data_type/node.h"
#include "coderep/data_type/output_mode.h"
#include "coderep/macros.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

const size_t FREQUENCY_DEFAULT_SIZE = 20;

struct frequency *frequency_alloc(struct allocator *allocator, struct string **name, size_t initial_capacity)
{
	if (initial_capacity == 0) initial_capacity = FREQUENCY_DEFAULT_SIZE;

	struct frequency *frequency = allocator->allocate(allocator->state, sizeof(struct frequency));
	struct node **nodes = allocator->allocate(allocator->state, sizeof(struct node *) * initial_capacity);

	if (!frequency || !nodes) goto defer;

	memset(nodes, 0, sizeof(struct node *) * initial_capacity);

	*frequency = (struct frequency) {
		.allocator = allocator,
		.buckets_count = initial_capacity,
		.buckets = nodes,
		.name = *name,
	};

	*name = NULL;
	
	return frequency;

defer:
	string_drop(name);
	allocator->deallocate(allocator->state, frequency);
	allocator->deallocate(allocator->state, nodes);

	MALLOC_ERR_EXIT("struct frequency");
}

bool frequency_has(struct frequency *self, const struct string *value)
{
	size_t bucket = string_hash(value) % self->buckets_count;
	struct node *node = self->buckets[bucket];

	if (node == NULL) return false;
	else return node_has(node, value);
}

bool frequency_increment(struct frequency *self, const struct string *value)
{
	size_t bucket = string_hash(value) % self->buckets_count;
	struct node *node = self->buckets[bucket];

	if (node == NULL) return false;
	else return node_increment(node, value);
}

void frequency_intern(struct frequency *self, const struct string *value)
{
	if (frequency_has(self, value)) frequency_increment(self, value);
	else {
		struct string *cloned = string_clone(value, self->allocator, true);
		frequency_push(self, &cloned);
	}
}

void frequency_push(struct frequency *self, struct string **value)
{
	size_t bucket = string_hash(*value) % self->buckets_count;
	struct node *node = self->buckets[bucket];

	size_t count = self->element_count;

	if (node == NULL) {
		node = node_alloc(self->allocator, value);
		self->buckets[bucket] = node;
	} else count = node_push(node, self->allocator, value, count);

	self->element_count = count;
	self->element_count++;

	*value = NULL;
}

struct stats frequency_calculate_stats(const struct frequency *self)
{
	if (self->element_count == 0) return (struct stats) { 0 };

	struct stats stats = (struct stats) { 
		.min = 0x7fffffff,
	};
	size_t acumulator = 0;

	for (size_t i = 0; i < self->buckets_count; i++) {
		struct node *node = self->buckets[i];

		while (node != NULL) {
			acumulator += node->count;

			if (node->count < stats.min) stats.min = node->count;
			if (node->count > stats.max) stats.max = node->count;
			if (node->count > stats.mode) {
				stats.mode = node->count;
				stats.mode_node = node;
			}
			// let skip median for now

			node = node->_next;
		}
	}

	stats.avg = (float)acumulator / self->element_count;

	return stats;
}

void frequency_print_stats(const struct frequency *self, enum output_mode output_mode, bool one_per_line)
{
	if (self->element_count == 0) return;

	struct stats stats = frequency_calculate_stats(self);

	const char *prefix = "\t\t";
	const char *sufix = ",\n";
	if (!one_per_line) {
		prefix = "";
		sufix = ", ";
	}

	if ((output_mode & OUTPUT_MODE_MIN) == OUTPUT_MODE_MIN) printf("%smin = %d%s", prefix, stats.min, sufix);
	if ((output_mode & OUTPUT_MODE_AVG) == OUTPUT_MODE_AVG) printf("%savg = %f%s", prefix, stats.avg, sufix);
	if ((output_mode & OUTPUT_MODE_MAX) == OUTPUT_MODE_MAX) printf("%smax = %d%s", prefix, stats.max, sufix);

	if ((output_mode & OUTPUT_MODE_MODE) == OUTPUT_MODE_MODE) {

		printf("%smode = \"", prefix);
		string_print(stats.mode_node->value);
		printf("\"(%d)%s", stats.mode_node->count, sufix);
	}
}

void frequency_print_raw(const struct frequency *self, enum output_mode output_mode)
{
	printf("(struct frequency) {\n\t.name = \"");
	string_print(self->name);
	puts("\",");
	printf("\t.buckets_count = %zu,\n", self->buckets_count);
	printf("\t.element_count = %zu,\n", self->element_count);

	printf("\t.buckets");
	if ((output_mode & OUTPUT_MIX_DISTRIBUTION) !=  0) {
		puts(" = [");
		bool show_value = (output_mode & OUTPUT_MODE_VALUE) == OUTPUT_MODE_VALUE;
		bool show_frequency = (output_mode & OUTPUT_MODE_FREQUENCY) == OUTPUT_MODE_FREQUENCY;

		for (size_t i = 0; i < self->buckets_count; i++) {
			struct node *node = self->buckets[i];

			printf("\t\t[%zu] = [", i);

			if (node != NULL) node_print(node, show_value, show_frequency);
			else printf("(!)");

			puts("]");
		}
		puts("\t]");
	} else puts(",");

	if ((output_mode & OUTPUT_MIX_STATS) != 0) {
		puts("\tstats {");
		frequency_print_stats(self, output_mode, true);
		puts("\t},");
	}

	puts("}");
}

void frequency_print_normal(const struct frequency *self, enum output_mode output_mode)
{
	string_print(self->name);
	printf(": ");
	if ((output_mode & OUTPUT_MIX_STATS) != 0) frequency_print_stats(self, output_mode, false);

	if ((output_mode & OUTPUT_MIX_DISTRIBUTION) !=  0) {
		putchar('[');
		bool first = true;
		bool show_value = (output_mode & OUTPUT_MODE_VALUE) == OUTPUT_MODE_VALUE;
		bool show_frequency = (output_mode & OUTPUT_MODE_FREQUENCY) == OUTPUT_MODE_FREQUENCY;
		for (size_t i = 0; i < self->buckets_count; i++) {
			struct node *node = self->buckets[i];

			if (node != NULL) {
				if (!first) printf(", ");
				node_print(node, show_value, show_frequency);

				first = false;
			}
		}
		putchar(']');
	}
	if ((output_mode & OUTPUT_MODE_COUNT) == OUTPUT_MODE_COUNT) printf("(%zu)", self->element_count);

	putchar('\n');
}

void frequency_print(const struct frequency *self, enum output_mode output_mode)
{
	if ((output_mode & OUTPUT_MODE_RAW) == OUTPUT_MODE_RAW) frequency_print_raw(self, output_mode);
	else frequency_print_normal(self, output_mode);
}

void frequency_drop(struct frequency **self)
{
	if (!self || !(*self)) return;
	struct frequency *tmp = *self;

	for (size_t i = 0; i < tmp->buckets_count; i++) node_drop(&(tmp->buckets[i]), tmp->allocator);

	tmp->allocator->deallocate(tmp->allocator->state, tmp->buckets);
	tmp->buckets = NULL;

	string_drop(&tmp->name);
	tmp->allocator->deallocate(tmp->allocator->state, *self);
	*self = NULL;
}

