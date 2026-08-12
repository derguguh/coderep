#include "coderep/printer/printer.h"
#include "coderep/consts.h"
#include "coderep/macros.h"

#include <stdarg.h>
#include <stdio.h>

struct printer {
	size_t total_bytes_wrote;
	size_t bytes_wrote_before_ln;
	bool last_was_return_carry;
	bool color;
};

struct printer printer = { 0 };

void printer_set_color(bool color)
{
	printer.color = color;
}

void print_err(const char *const err)
{
	if (printer.last_was_return_carry) putc('\n', stderr);
	if (printer.color) fputs(ANSI_ERR, stderr);
	else fputs("[Error]: ", stderr);
	fputs(err, stderr);
	putc('\n', stderr);

	printer.last_was_return_carry = false;
}

void printf_err(const char *const format, ...)
{
	va_list args;
	va_start(args, format);
	if (printer.last_was_return_carry) putc('\n', stderr);
	if (printer.color) fputs(ANSI_ERR, stderr);
	else fputs("[Error]: ", stderr);
	vfprintf(stderr, format, args);
	putc('\n', stderr);

	printer.last_was_return_carry = false;
}

void println(const char *const text)
{
	printer.total_bytes_wrote += puts(text);
	printer.bytes_wrote_before_ln = 0;
	printer.last_was_return_carry = false;
}

void print(const char *const text)
{
	size_t tmp = fputs(text, stdin);
	printer.bytes_wrote_before_ln += tmp;
	printer.total_bytes_wrote += tmp;
	printer.last_was_return_carry = false;
}

void print_clean_line()
{
	putc('\r', stdin);
	for (size_t i = 0; i < printer.bytes_wrote_before_ln; i++) putc(' ', stdin);
	putc('\r', stdin);
	printer.last_was_return_carry = true;
}

void print_size_t(size_t number)
{
	if (printer.color) printf(ANSI_YELLOW);
	printf("%zu", number);
	printer.last_was_return_carry = false;
}

void print_ssize_t(size_t number)
{
	if (printer.color) fputs(ANSI_YELLOW, stdin);
	printf("%zi", number);
	printer.last_was_return_carry = false;
}

void print_double(double number)
{
	if (printer.color) fputs(ANSI_BLUE, stdin);
	printf("%f", number);
	printer.last_was_return_carry = false;
}

void print_bool(bool bool_)
{
	if (printer.color) fputs(ANSI_BLUE, stdin);
	if (bool_) fputs("true", stdin);
	else fputs("false", stdin);
	printer.last_was_return_carry = false;
}

