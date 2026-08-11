#include "coderep/data_type/frequency.h"

#include "coderep/allocator/allocator.h"
#include "coderep/consts.h"
#include "coderep/data_type/node.h"
#include "coderep/data_type/output_mode.h"
#include "coderep/macros.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct frequency {
	struct allocator *allocator;
	struct node **buckets;
	size_t buckets_count;
	size_t element_count;
	size_t count_total;
};

struct stats {
	bool have_something;
	ssize_t min;
	float avg;
	ssize_t max;
	ssize_t mode;
	const struct node *mode_node;
	float median;
	const struct node *media_node_low;
	const struct node *media_node_high;
};

const size_t FREQUENCY_DEFAULT_SIZE = 20;

struct frequency *frequency_alloc(struct allocator *allocator, size_t initial_capacity)
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
	};

	return frequency;

defer:
	allocator->deallocate(allocator->state, frequency);
	allocator->deallocate(allocator->state, nodes);

	ALLOC_ERR_EXIT("struct frequency");
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
	else {
		self->count_total++;
		return node_increment(node, value);
	}
}

bool frequency_intern(struct frequency *self, const struct string *value)
{
	size_t index = string_hash(value) % self->buckets_count;
	struct node *bucket = self->buckets[index];

	self->count_total++;
	if (bucket == NULL) {
		struct string *cloned = string_clone(self->allocator, value, true);
		self->buckets[index] = node_alloc(self->allocator, &cloned);
		self->element_count++;
		return true;
	} else {
		struct node *node = node_find(bucket, value);
		if (node == NULL) {
			struct string *cloned = string_clone(self->allocator, value, true);
			node_push(bucket, self->allocator, &cloned);
			self->element_count++;
			return true;
		} else {
			node->count++;
			return false;
		}
	}
}

void frequency_push(struct frequency *self, struct string **value)
{
	size_t index = string_hash(*value) % self->buckets_count;
	struct node *bucket = self->buckets[index];

	if (bucket == NULL) {
		self->buckets[index] = node_alloc(self->allocator, value);
		self->element_count++;
	} else if (node_push(bucket, self->allocator, value)->count == 1) self->element_count++;

	self->count_total++;
	*value = NULL;
}

struct stats frequency_calculate_stats(const struct frequency *self)
{
	if (self->element_count == 0) return (struct stats) { 0 };

	struct stats stats = (struct stats) { 
		.min = 0x7fffffff,
	};

	for (size_t i = 0; i < self->buckets_count; i++) {
		struct node *node = self->buckets[i];

		while (node != NULL) {

			if (node->count < stats.min) stats.min = node->count;
			if (node->count > stats.max) stats.max = node->count;
			if (node->count > stats.mode) {
				stats.mode = node->count;
				stats.mode_node = node;
			}
			// TODO: median

			node = node->_next;
		}
	}

	if (self->element_count == 0) stats.avg = 0;
	else stats.avg = (float)self->count_total / self->element_count;

	return stats;
}

void frequency_print_stats(const struct frequency *self, enum output_mode output_mode, bool one_per_line, bool do_color)
{
	if (self->element_count == 0) return;

	struct stats stats = frequency_calculate_stats(self);

	const char *number_color = do_color ? ANSI_YELLOW : "";
	const char *reset_color = do_color ? ANSI_RESET : "";
	const char *prefix = "\t\t";
	const char *sufix = ",\n";
	if (!one_per_line) {
		prefix = "";
		sufix = ", ";
	}

	if ((output_mode & OUTPUT_MODE_MIN) == OUTPUT_MODE_MIN) printf("%smin = %s%zi%s%s", prefix, number_color, stats.min, reset_color, sufix);
	if ((output_mode & OUTPUT_MODE_AVG) == OUTPUT_MODE_AVG) printf("%savg = %s%f%s%s", prefix, number_color, stats.avg, reset_color, sufix);
	if ((output_mode & OUTPUT_MODE_MAX) == OUTPUT_MODE_MAX) printf("%smax = %s%zi%s%s", prefix, number_color, stats.max, reset_color, sufix);


	if ((output_mode & OUTPUT_MODE_MODE) == OUTPUT_MODE_MODE) {

		printf("%smode = \"", prefix);
		if (do_color) printf(ANSI_CYAN);
		string_print(stats.mode_node->value);
		if (do_color) printf(ANSI_RESET);
		printf("\"(");
		if (do_color) printf(ANSI_YELLOW);
		printf("%zi", stats.mode_node->count);
		if (do_color) printf(ANSI_RESET);
		printf(")%s", sufix);
	}
}

void frequency_print_raw(const struct frequency *self, enum output_mode output_mode, bool do_color)
{
	const char *number_color = do_color ? ANSI_YELLOW : "";
	const char *reset_color = do_color ? ANSI_RESET : "";
	printf("(struct frequency) {\n");
	printf("\t.buckets_count = %s%zu,%s\n", number_color, self->buckets_count, reset_color);
	printf("\t.element_count = %s%zu,%s\n", number_color, self->element_count, reset_color);

	printf("\t.buckets");
	if ((output_mode & OUTPUT_MIX_DISTRIBUTION) !=  0) {
		puts(" = [");
		bool show_value = (output_mode & OUTPUT_MODE_VALUE) == OUTPUT_MODE_VALUE;
		bool show_frequency = (output_mode & OUTPUT_MODE_FREQUENCY) == OUTPUT_MODE_FREQUENCY;

		for (size_t i = 0; i < self->buckets_count; i++) {
			struct node *node = self->buckets[i];

			printf("\t\t[%zu] = [", i);

			if (node != NULL) node_print(node, show_value, show_frequency, do_color);
			else printf("(!)");

			puts("]");
		}
		puts("\t]");
	} else puts(",");

	if ((output_mode & OUTPUT_MIX_STATS) != 0) {
		puts("\tstats {");
		frequency_print_stats(self, output_mode, true, do_color);
		puts("\t},");
	}

	putchar('}');
}

void frequency_print_normal(const struct frequency *self, enum output_mode output_mode, bool do_color)
{
	if ((output_mode & OUTPUT_MIX_STATS) != 0) frequency_print_stats(self, output_mode, false, do_color);

	if ((output_mode & OUTPUT_MIX_DISTRIBUTION) !=  0) {
		putchar('[');
		bool first = true;
		bool show_value = (output_mode & OUTPUT_MODE_VALUE) == OUTPUT_MODE_VALUE;
		bool show_frequency = (output_mode & OUTPUT_MODE_FREQUENCY) == OUTPUT_MODE_FREQUENCY;
		for (size_t i = 0; i < self->buckets_count; i++) {
			struct node *node = self->buckets[i];

			if (node != NULL) {
				if (!first) printf(", ");
				node_print(node, show_value, show_frequency, do_color);

				first = false;
			}
		}
		putchar(']');
	}
	if ((output_mode & OUTPUT_MODE_COUNT) == OUTPUT_MODE_COUNT) {
		const char *number_color = do_color ? ANSI_YELLOW : "";
		const char *reset_color = do_color ? ANSI_RESET : "";
		printf("(unique: %s%zu%s, total: %s%zu%s)",
				number_color, self->element_count, reset_color,
				number_color, self->count_total, reset_color);
	}
}

void frequency_print(const struct frequency *self, enum output_mode output_mode, bool do_color)
{
	if ((output_mode & OUTPUT_MODE_RAW) == OUTPUT_MODE_RAW) frequency_print_raw(self, output_mode, do_color);
	else frequency_print_normal(self, output_mode, do_color);
}

void frequency_drop(struct frequency **self)
{
	if (!self || !(*self)) return;
	struct frequency *tmp = *self;

	for (size_t i = 0; i < tmp->buckets_count; i++) node_drop(&(tmp->buckets[i]), tmp->allocator);

	tmp->allocator->deallocate(tmp->allocator->state, tmp->buckets);
	tmp->buckets = NULL;

	tmp->allocator->deallocate(tmp->allocator->state, *self);
	*self = NULL;
}

