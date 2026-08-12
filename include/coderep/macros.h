#ifndef CODEREP_MACROS_H
#define CODEREP_MACROS_H

#include "coderep/printer/printer.h"
#include "coderep/consts.h"

#include <stdio.h>

#define ANSI_F(color, text)	color ANSI_BOLD "[" text "]: " ANSI_RESET
#define ANSI_DEBUG		ANSI_F(ANSI_DEBUG, "Debug")
#define ANSI_LOG		ANSI_F(ANSI_CYAN, "Log")
#define ANSI_WARN		ANSI_F(ANSI_YELLOW, "Warn")
#define ANSI_ERR		ANSI_F(ANSI_RED, "Error")

#define PRINT_ANSI(out, type, text)		fprintf(out, type text ANSI_RESET "\n")
#define PRINTF_ANSI(out, type, text, ...)	fprintf(out, type text ANSI_RESET "\n", __VA_ARGS__)

#ifdef DEBUG
#define LOG(log)		PRINT_ANSI(stdout, ANSI_LOG, log)
#define LOGF(log, ...)		PRINTF_ANSI(stdout, ANSI_LOG, log, __VA_ARGS__)
#else
#define LOG(log)
#define LOGF(log, ...)
#endif

#define ALLOC_ERR_EXIT(name) do { \
		print_err("Couldn't allocate memory for " name); \
		exit(ERROR_CODE_MEMORY_ALLOCATION); \
		return NULL; \
} while (0)

#define ARRAY_SIZE(array, struct_) (sizeof(array) / sizeof(struct_))

#endif
