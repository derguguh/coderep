#ifndef CODEREP_TOOLS_ARGUMENT_PARSER_H
#define CODEREP_TOOLS_ARGUMENT_PARSER_H

struct args {

};

struct args *args_new(char *argv[], int args);

void args_drop(struct args **self);

#endif
