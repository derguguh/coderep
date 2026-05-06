#ifndef CODEREP_CONSTS_H
#define CODEREP_CONSTS_H

#define ANSI_ESC	"\033["

// Formats
#define ANSI_BOLD	"1m"

// Colors
#define ANSI_BLACK	"30m"
#define ANSI_RED	"31m"
#define ANSI_GREEN	"32m"
#define ANSI_YELLOW	"33m"
#define ANSI_BLUE	"34m"
#define ANSI_MAGENTA	"35m"
#define ANSI_CYAN	"36m"
#define ANSI_WHITE	"37m"
#define ANSI_DEFAULT	"39m"

#define ANSI_RESET	"0m"

enum error_code {
	// General
	ERROR_CODE_SUCCESS,
	ERROR_CODE_UNKNOWN,
	ERROR_CODE_UNKNOWN_ARGUMENT,
	// Memory
	ERROR_CODE_MEMORY_ALLOCATION,
	ERROR_CORE_MEMORY_REALLOCATION,
	// File handling
	ERROR_CODE_UNKNOWN_FILE,
	ERROR_CODE_EMPTY_FILE,
};

#endif

