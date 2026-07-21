#include "coderep/data_type/statistics.h"
#include "coderep/data_type/output_mode.h"
#include <stdio.h>

#define DEFINE_STATISTICS_FOR(type, format) void statistics_##type##_push(struct statistics_##type *self, type value) { \
	if (value < self->min || self->count == 0) self->min = value; \
	if (value > self->max) self->max = value; \
\
	self->total += value; \
	self->count++; \
} \
\
void statistics_##type##_print(const struct statistics_##type *self, enum output_mode mode) \
{ \
	type avg; \
	if (self->count == 0) avg = 0; \
	else avg = self->total / (type)self->count; \
\
	if ((mode & OUTPUT_MODE_RAW) == OUTPUT_MODE_RAW) { \
		printf("(struct statistics_" #type ") {\n"); \
		printf("\t.min = " format ",\n", self->min); \
		if ((mode & OUTPUT_MODE_AVG) == OUTPUT_MODE_AVG) printf("\tavg = " format ",\n", avg); \
		printf("\t.max = " format ", \n", self->max); \
		printf("\t.total = " format ", \n", self->total); \
		printf("\t.count = %zu,\n", self->count); \
		printf("}"); \
	} else { \
		if ((mode & OUTPUT_MODE_MIN) == OUTPUT_MODE_MIN) printf("min = " format ", ", self->min); \
		if ((mode & OUTPUT_MODE_AVG) == OUTPUT_MODE_AVG) printf("avg = " format ", ", avg); \
		if ((mode & OUTPUT_MODE_MAX) == OUTPUT_MODE_MAX) printf("max = " format ", ", self->max); \
		if ((mode & OUTPUT_MODE_COUNT) == OUTPUT_MODE_COUNT) printf("count = %zu, ", self->count); \
		printf("total: " format "", self->total); \
	} \
}\

DEFINE_STATISTICS_FOR(double, "%.5f");
DEFINE_STATISTICS_FOR(ssize_t, "%zi");
DEFINE_STATISTICS_FOR(size_t, "%zu");

