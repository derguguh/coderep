#ifndef CODEREP_DATA_TYPE_STATISTICS_H
#define CODEREP_DATA_TYPE_STATISTICS_H

#include "coderep/data_type/output_mode.h"

#include <sys/types.h>

struct statistics_double {
	double min;
	double max;
	double total;
	size_t count;
};

struct statistics_ssize_t {
	ssize_t min;
	ssize_t max;
	ssize_t total;
	size_t count;
};

struct statistics_size_t {
	size_t min;
	size_t max;
	size_t total;
	size_t count;
};

void statistics_double_push(struct statistics_double *self, double value);
void statistics_ssize_t_push(struct statistics_ssize_t *self, ssize_t value);
void statistics_size_t_push(struct statistics_size_t *self, size_t value);

void statistics_double_print(const struct statistics_double *self, enum output_mode mode);
void statistics_ssize_t_print(const struct statistics_ssize_t *self, enum output_mode mode);
void statistics_size_t_print(const struct statistics_size_t *self, enum output_mode mode);

#endif
