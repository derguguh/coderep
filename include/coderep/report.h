#ifndef CODEREP_REPORT_H
#define CODEREP_REPORT_H

#include "coderep/cli/data.h"
#include "coderep/data_type/brute.h"
#include "coderep/data_type/frequency.h"

#include <stddef.h>

struct report {
	struct brute **brutes;
	size_t brutes_l;
	size_t brutes_c;
	struct frequency **frequencies;
	size_t frequencies_l;
	size_t frequencies_c;
};

extern const size_t REPORT_DEFAULT_CAPACITY;

struct report *report_alloc();

void report_push_brute(struct report *self, struct brute **brute);
void report_push_frequency(struct report *self, struct frequency **frequency);

void report_print(struct report *self, struct data *output_options);

void report_drop(struct report **self);

#endif
