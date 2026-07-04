#include "coderep/report.h"

#include "coderep/data_type/frequency.h"
#include "coderep/macros.h"

#include <stdlib.h>
#include <string.h>

const size_t _DEFAULT_CAPACITY = 20;

enum report_entry_type {
	REPORT_ENTRY_TYPE_FREQUENCY,
	REPORT_ENTRY_TYPE_STATISTICS_DOUBLE,
	REPORT_ENTRY_TYPE_STATISTICS_SSIZE_T,
	REPORT_ENTRY_TYPE_STRING,
	REPORT_ENTRY_TYPE_DOUBLE,
	REPORT_ENTRY_TYPE_SSIZE_T,
	REPORT_ENTRY_TYPE_BOOL,
};

struct report_entry {
	enum report_entry_type type;
	struct string *name;
	union {
		struct frequency *frequency;
		struct string *string;
		double double_;
		ssize_t ssize_t_;
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

	MALLOC_ERR_EXIT("struct report (backend outputs)");
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

void report_print(struct report *self, struct data *output_options)
{
	printf("\n");
}

void report_drop(struct report **self)
{
	if (!self || !(*self)) return;
	struct allocator *allocator = (*self)->allocator;
	struct report *tmp = *self;

	allocator->deallocate(allocator->state, tmp);

	allocator->deallocate(allocator->state, tmp);
	*self = NULL;
}

