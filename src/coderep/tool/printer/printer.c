#include "coderep/tool/printer/printer.h"
#include "coderep/consts.h"
#include "coderep/macros.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

struct printer {
	size_t total_bytes_wrote;
	size_t bytes_on_line;
	bool color;
};

struct printer printer = { 0 };

// ------------ Toggles ------------
void printer_set_color(bool color)
{
	printer.color = color;
}

// ------------ Bases ------------
void print_err_prefix()
{
	if (printer.bytes_on_line != 0) putc('\n', stdout);
	if (printer.color) fputs(ANSI_ERR, stderr);
	else fputs("[Error]: ", stderr);
}

void print_err(const char *const err)
{
	print_err_prefix();

	fputs(err, stderr);

	if (printer.color) fputs(ANSI_RESET "\n", stderr);
	else fputs("\n", stderr);

	printer.bytes_on_line = 0;
}

void printf_err(const char *const format, ...)
{
	va_list args;
	va_start(args, format);

	print_err_prefix();

	vfprintf(stderr, format, args);

	if (printer.color) fputs(ANSI_RESET "\n", stderr);
	else fputs("\n", stderr);

	printer.bytes_on_line = 0;
}

void print_clean_line()
{
	putc('\r', stdout);
	for (size_t i = 0; i < printer.bytes_on_line; i++) putc(' ', stdout);
	putc('\r', stdout);
	printer.bytes_on_line = 0;
}



void print(const char *const text)
{
	fputs(text, stdout);
	size_t tmp = strlen(text);

	printer.total_bytes_wrote += tmp;

	for (size_t i = tmp - 1; i >= 0 && i < tmp; i--) {
		if (text[i] == '\n') {
			printer.bytes_on_line = tmp - i - 1;
			return;
		}
	}

	printer.bytes_on_line += tmp;
}

void println(const char *const text)
{
	puts(text);
	printer.total_bytes_wrote += strlen(text) + 1;
	printer.bytes_on_line = 0;
}

// ------------ Printing Data ------------
void print_bool(bool bool_)
{
	size_t tmp = 0;

	const char *text; 
	if (bool_) text = "true";
	else text = "false";

	if (printer.color) tmp += printf(ANSI_BLUE "%s" ANSI_RESET, text);
	else tmp += printf("%s", text);

	printer.bytes_on_line += tmp;
	printer.total_bytes_wrote += tmp;
}

void print_char(char c)
{
	size_t tmp = 0;

	if (printer.color) tmp += printf(ANSI_GREEN "%c" ANSI_RESET, c);
	else tmp += printf("%c", c);

	printer.total_bytes_wrote += tmp;
	if (c == '\n') printer.bytes_on_line = 0;
	else printer.bytes_on_line += tmp;
}



void print_cstr(const char *text, size_t length)
{
	size_t tmp = 0;

	if (printer.color) tmp += printf(ANSI_GREEN "%*s" ANSI_RESET, (int)length, text);
	else tmp += printf("%*s", (int)length, text);

	printer.bytes_on_line += tmp;
	printer.total_bytes_wrote += tmp;
}



void print_pointer(void *ptr)
{
	size_t tmp = 0;

	if (printer.color) tmp += printf(ANSI_MAGENTA "%p" ANSI_RESET, ptr);
	else tmp += printf("%p", ptr);

	printer.bytes_on_line += tmp;
	printer.total_bytes_wrote += tmp;
}



//  Numbers
void print_double(double number)
{
	size_t tmp = 0;

	if (printer.color) tmp += printf(ANSI_YELLOW "%.3f" ANSI_RESET, number);
	else tmp += printf("%.3f", number);

	printer.bytes_on_line += tmp;
	printer.total_bytes_wrote += tmp;
}

void print_size_t(size_t number)
{
	size_t tmp = 0;

	if (printer.color) tmp += printf(ANSI_YELLOW "%zu" ANSI_RESET, number);
	else tmp += printf("%zu", number);

	printer.bytes_on_line += tmp;
	printer.total_bytes_wrote += tmp;
}

void print_ssize_t(size_t number)
{
	size_t tmp = 0;

	if (printer.color) tmp += printf(ANSI_YELLOW "%zi" ANSI_RESET, number);
	else tmp += printf("%zi", number);

	printer.bytes_on_line += tmp;
	printer.total_bytes_wrote += tmp;
}

//  Code Report specifics
void print_string(struct string *text)
{
	size_t tmp = 0;
	if (printer.color) tmp = printf(ANSI_GREEN);
	string_print(text);
	if (printer.color) tmp += printf(ANSI_RESET);

	printer.bytes_on_line += text->length + tmp;
	printer.total_bytes_wrote += text->length + tmp;
}
