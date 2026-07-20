#include "coderep/report.h"

#include "coderep/consts.h"
#include "coderep/data_type/frequency.h"
#include "coderep/data_type/output_mode.h"
#include "coderep/data_type/statistics.h"
#include "coderep/macros.h"

#include <stdlib.h>
#include <string.h>

const size_t _DEFAULT_CAPACITY = 20;

enum report_entry_type {
	REPORT_ENTRY_TYPE_FREQUENCY,
	REPORT_ENTRY_TYPE_STATISTICS_DOUBLE,
	REPORT_ENTRY_TYPE_STATISTICS_SSIZE_T,
	REPORT_ENTRY_TYPE_STATISTICS_SIZE_T,
	REPORT_ENTRY_TYPE_STRING,
	REPORT_ENTRY_TYPE_DOUBLE,
	REPORT_ENTRY_TYPE_SSIZE_T,
	REPORT_ENTRY_TYPE_SIZE_T,
	REPORT_ENTRY_TYPE_BOOL,
};

struct report_entry {
	enum report_entry_type type;
	struct string *name;
	union {
		struct frequency *frequency;
		struct string *string;
		struct statistics_double statistics_double;
		struct statistics_ssize_t statistics_ssize_t;
		struct statistics_size_t statistics_size_t;
		double double_;
		ssize_t ssize_t_;
		size_t size_t_;
		bool bool_;
	};
};

struct report {
	struct allocator *allocator;
	struct report_entry *entries;
	size_t entries_l;
	size_t entries_c;
};

struct report *report_alloc(struct allocator *allocator)
{
	struct report_entry *entries = allocator->allocate(allocator->allocate, sizeof(struct report_entry) * _DEFAULT_CAPACITY);
	struct report *self = allocator->allocate(allocator->allocate, sizeof(struct report));

	if (!self || !entries) goto defer;

	*self = (struct report) {
		.allocator = allocator,
		.entries = entries,
		.entries_c = _DEFAULT_CAPACITY,
	};

	return self;
defer:
	free(self);
	free(entries);

	ALLOC_ERR_EXIT("struct report (backend outputs)");
}

static inline void report_expand(struct report *self)
{
	self->entries_c *= 2;
	struct report_entry *tmp = self->allocator->reallocate(self->allocator->state, self->entries, sizeof(struct report) * self->entries_c);

	if (!tmp) goto defer;

	self->entries = tmp;

	return;
defer:
	PRINT_ERR("Couldn't reallocate memory to resize a struct report");
	exit(ERROR_CODE_MEMORY_REALLOCATION);
}

static inline void report_push(struct report *self, struct report_entry value)
{
	if (self->entries_l >= self->entries_c) report_expand(self);

	self->entries[self->entries_l] = value;
	self->entries_l++;
}

void report_push_file_report(struct report *report, struct file_report *context)
{
	report_push_size_t(report, "files.read.total", context->total_reads);
	report_push_size_t(report, "files.read.successful", context->total_reads - context->failed_reads);
	report_push_size_t(report, "files.read.failed", context->failed_reads);
	report_push_statistics_size_t(report, "files.read.bytes", &context->bytes_read);

	if (context->is_binary) {
	} else {
		report_push_statistics_size_t(report, "files.lines", &context->total_line);
	}

	report_push_bool(report, "file.is_binary", context->is_binary);
	report_push_statistics_double(report, "backend.time.init", &context->total_init_time);
	report_push_statistics_double(report, "backend.time.parsing", &context->total_file_parse_time);
	report_push_statistics_double(report, "backend.time.deinit", &context->total_deinit_time);
}

void report_push_cstr(struct report *self, const char *const name, const char *const value)
{
	struct report_entry entry = (struct report_entry){ 
		.name = string_from_cstr(self->allocator, name),
		.type = REPORT_ENTRY_TYPE_STRING,
		.string = string_from_cstr(self->allocator, value),
	};
	report_push(self, entry);
}

void report_push_string(struct report *self, const char *const name, struct string **value)
{
	struct report_entry entry = (struct report_entry){ 
		.name = string_from_cstr(self->allocator, name),
		.type = REPORT_ENTRY_TYPE_STRING,
		.string = *value,
	};
	report_push(self, entry);
	*value = NULL;
}

void report_push_bool(struct report *self, const char *const name, bool value)
{
	struct report_entry entry = (struct report_entry){ 
		.name = string_from_cstr(self->allocator, name),
		.type = REPORT_ENTRY_TYPE_BOOL,
		.bool_ = value,
	};
	report_push(self, entry);
}

void report_push_ssize_t(struct report *self, const char *const name, ssize_t value)
{
	struct report_entry entry = (struct report_entry){ 
		.name = string_from_cstr(self->allocator, name),
		.type = REPORT_ENTRY_TYPE_SSIZE_T,
		.ssize_t_ = value,
	};
	report_push(self, entry);
}

void report_push_size_t(struct report *self, const char *const name, size_t value)
{
	struct report_entry entry = (struct report_entry){ 
		.name = string_from_cstr(self->allocator, name),
		.type = REPORT_ENTRY_TYPE_SIZE_T,
		.size_t_ = value,
	};
	report_push(self, entry);
}

void report_push_double(struct report *self, const char *const name, double value)
{
	struct report_entry entry = (struct report_entry){ 
		.name = string_from_cstr(self->allocator, name),
		.type = REPORT_ENTRY_TYPE_DOUBLE,
		.double_ = value,
	};
	report_push(self, entry);
}


void report_push_frequency(struct report *self, const char *const name, struct frequency **value)
{
	struct report_entry entry = (struct report_entry){ 
		.name = string_from_cstr(self->allocator, name),
		.type = REPORT_ENTRY_TYPE_FREQUENCY,
		.frequency = *value,
	};
	report_push(self, entry);
	*value = NULL;
}

void report_push_statistics_double(struct report *self, const char *const name, struct statistics_double *value)
{
	struct report_entry entry = (struct report_entry){ 
		.name = string_from_cstr(self->allocator, name),
		.type = REPORT_ENTRY_TYPE_STATISTICS_DOUBLE,
		.statistics_double = *value,
	};
	report_push(self, entry);
}

void report_push_statistics_ssize_t(struct report *self, const char *const name, struct statistics_ssize_t *value)
{
	struct report_entry entry = (struct report_entry){ 
		.name = string_from_cstr(self->allocator, name),
		.type = REPORT_ENTRY_TYPE_STATISTICS_SSIZE_T,
		.statistics_ssize_t = *value,
	};
	report_push(self, entry);
}

void report_push_statistics_size_t(struct report *self, const char *const name, struct statistics_size_t *value)
{
	struct report_entry entry = (struct report_entry){ 
		.name = string_from_cstr(self->allocator, name),
		.type = REPORT_ENTRY_TYPE_STATISTICS_SIZE_T,
		.statistics_size_t = *value,
	};
	report_push(self, entry);
}

void report_entry_print(struct report_entry *entry, struct data *output_options)
{
	string_print(entry->name);
	printf(": ");
	
	if (entry->type == REPORT_ENTRY_TYPE_FREQUENCY && output_options->frequency != OUTPUT_MODE_NONE) {
		frequency_print(entry->frequency, output_options->frequency);
	} else if (entry->type == REPORT_ENTRY_TYPE_STATISTICS_DOUBLE && output_options->statistic != OUTPUT_MODE_NONE) {
		statistics_double_print(&entry->statistics_double, output_options->statistic);
	} else if (entry->type == REPORT_ENTRY_TYPE_STATISTICS_SSIZE_T && output_options->statistic != OUTPUT_MODE_NONE) {
		statistics_ssize_t_print(&entry->statistics_ssize_t, output_options->statistic);
	} else if (entry->type == REPORT_ENTRY_TYPE_STATISTICS_SIZE_T && output_options->statistic != OUTPUT_MODE_NONE) {
		statistics_size_t_print(&entry->statistics_size_t, output_options->statistic);
	} else if (output_options->brute != OUTPUT_MODE_NONE) {
		if ((output_options->brute & OUTPUT_MODE_RAW) == OUTPUT_MODE_RAW) printf("(brute) ");
		switch (entry->type) {
			case REPORT_ENTRY_TYPE_STRING:
				printf(ANSI_CYAN);
				string_print(entry->string);
			break;
			case REPORT_ENTRY_TYPE_DOUBLE:	printf(ANSI_YELLOW "%.5f", entry->double_);	break;
			case REPORT_ENTRY_TYPE_SSIZE_T:	printf(ANSI_YELLOW "%zi", entry->ssize_t_);	break;
			case REPORT_ENTRY_TYPE_SIZE_T:	printf(ANSI_YELLOW "%zu", entry->size_t_);	break;
			case REPORT_ENTRY_TYPE_BOOL:
				if (entry->bool_ == true) printf(ANSI_BLUE "true");
				else printf(ANSI_BLUE "false");
			break;
			default: break;
		}
	}
	printf(ANSI_RESET "\n");
}

void report_print(struct report *self, struct data *output_options)
{
	printf("\n");

	for (size_t i = 0; i < self->entries_l; i++) {
		report_entry_print(&self->entries[i], output_options);
	}
}

void report_drop(struct report **self)
{
	if (!self || !(*self)) return;
	struct allocator *allocator = (*self)->allocator;
	struct report *tmp = *self;

	for (size_t i = 0; i < tmp->entries_l; i++) {
		string_drop(&tmp->entries[i].name);

		switch (tmp->entries[i].type) {
			case REPORT_ENTRY_TYPE_FREQUENCY:
				frequency_drop(&tmp->entries[i].frequency);
			break;
			case REPORT_ENTRY_TYPE_STRING:
				string_drop(&tmp->entries[i].string);
			break;
			default: break;
		}
	}

	allocator->deallocate(allocator->state, tmp->entries);
	tmp->entries = NULL;
	allocator->deallocate(allocator->state, tmp);
	*self = NULL;
}

