#ifndef CODEREP_REPORT_H
#define CODEREP_REPORT_H

#include "coderep/cli/data.h"
#include "coderep/data_type/frequency.h"
#include "coderep/data_type/statistics.h"

#include <stddef.h>

struct report;

struct file_report {
	struct statistics_double total_init_time;
	struct statistics_double total_file_parse_time;
	struct statistics_double total_deinit_time;
	// General
	struct statistics_size_t bytes_read;
	size_t total_reads;
	size_t failed_reads;
	bool is_binary;
	// Binary (struct file_buffer) only
	// Standard (struct file) only
	struct statistics_size_t total_line;
};

extern const size_t REPORT_DEFAULT_CAPACITY;

struct report *report_alloc(struct allocator *allocator);

void report_push_file_report(struct report *report, struct file_report *context);
void report_push_cstr(struct report *self, const char *const name, const char *const value);
void report_push_string(struct report *self, const char *const name, struct string **value);
void report_push_bool(struct report *self, const char *const name, bool value);
void report_push_ssize_t(struct report *self, const char *const name, ssize_t value);
void report_push_size_t(struct report *self, const char *const name, size_t value);
void report_push_double(struct report *self, const char *const name, double value);
void report_push_frequency(struct report *self, const char *const name, struct frequency **value);
void report_push_statistics_double(struct report *self, const char *const name, struct statistics_double *value);
void report_push_statistics_ssize_t(struct report *self, const char *const name, struct statistics_ssize_t *value);
void report_push_statistics_size_t(struct report *self, const char *const name, struct statistics_size_t *value);

void report_print(struct report *self, struct data *output_options);

void report_drop(struct report **self);

#endif
