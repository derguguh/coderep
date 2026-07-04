#ifndef CODEREP_REPORT_H
#define CODEREP_REPORT_H

#include "coderep/cli/data.h"
#include "coderep/data_type/frequency.h"

#include <stddef.h>

struct report;

extern const size_t REPORT_DEFAULT_CAPACITY;

struct report *report_alloc(struct allocator *allocator);

void report_push_cstr(struct report *self, const char *const name, const char *const value);
void report_push_string(struct report *self, const char *const name, struct string **value);
void report_push_bool(struct report *self, const char *const name, bool value);
void report_push_ssize_t(struct report *self, const char *const name, ssize_t value);
void report_push_double(struct report *self, const char *const name, double value);
void report_push_frequency(struct report *self, const char *const name, struct frequency **value);

void report_print(struct report *self, struct data *output_options);

void report_drop(struct report **self);

#endif
