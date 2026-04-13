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

#define ANSI_ERR	ANSI_ESC ANSI_RED ANSI_ESC ANSI_BOLD "[Error] " ANSI_ESC ANSI_WHITE

enum error_code {
	ERROR_CODE_SUCCESS,
	ERROR_CODE_UNKNOWN,
	ERROR_UNKOWN_ARGUMENT,
	ERROR_CODE_MEMORY_ALLOCATION,
};

#endif

