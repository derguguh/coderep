#ifndef CODEREP_DATA_TYPE_STATISTICS_H
#define CODEREP_DATA_TYPE_STATISTICS_H

#include "coderep/data_type/output_mode.h"

#include <sys/types.h>

struct statistics_double {
	double min;
	double max;
	double total;
	size_t amount;
};

struct statistics_ssize_t {
	ssize_t min;
	ssize_t max;
	ssize_t total;
	size_t amount;
};

void statistics_double_push(struct statistics_double *statistics, double value);
void statistics_ssize_t_push(struct statistics_ssize_t *statistics, ssize_t value);

void statistics_double_print(struct statistics_double *statistics, enum output_mode mode);
void statistics_ssize_t_print(struct statistics_ssize_t *statistics, enum output_mode mode);

#endif
