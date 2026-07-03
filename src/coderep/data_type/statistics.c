#include "coderep/data_type/statistics.h"

void statistics_double_push(struct statistics_double *self, double value)
{
	if (value < self->min || self->amount == 0) self->min = value;
	if (value > self->max) self->max = value;

	self->total += value;
	self->amount++;
}

void statistics_ssize_t_push(struct statistics_ssize_t *self, ssize_t value)
{
	if (value < self->min || self->amount == 0) self->min = value;
	if (value > self->max) self->max = value;

	self->total += value;
	self->amount++;
}



void statistics_double_print(struct statistics_double *statistics, enum output_mode mode)
{

}

void statistics_ssize_t_print(struct statistics_ssize_t *statistics, enum output_mode mode)
{

}


