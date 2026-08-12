#ifndef CODEREP_PRINTER_PRINTER_H
#define CODEREP_PRINTER_PRINTER_H

#include "coderep/data_type/string.h"
#include <stdbool.h>
#include <stddef.h>

// checker for "format" attribute
#ifndef __has_attribute
#	define __has_attribute(x) 0
#endif

#if __has_attribute(format) || defined(__GNUC__)
	#define FORMAT_ATTR(format_id, va_id) __attribute__((format(printf, format_id, va_id)))
#else
	#define FORMAT_ATTR(format_id, va_id)
#endif

struct printer;

void printer_set_color(bool color);

void print_err(const char *const err);
void printf_err(const char *const format, ...) FORMAT_ATTR(1, 2);
void print_set_color(bool color);
void println(const char *const text);
void print_line();
void print(const char *const text);
void print_clean_line();
void print_cstr(const char *text, size_t length);
void print_string(struct string *text);
void print_pointer(void *ptr);
void print_char(char c);
void print_size_t(size_t number);
void print_ssize_t(size_t number);
void print_double(double number);
void print_bool(bool bool_);

#endif
