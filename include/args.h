#ifndef UNEBCC_ARGS_H
#define UNEBCC_ARGS_H

#include <stdbool.h>

typedef struct ArgsContext {
	const char *filename;
	bool print_lexer;
	bool print_parser;
	bool print_ir;
	bool print_machine_ir;

	bool ok;
}ArgsContext;

ArgsContext args_parse(int argc, char **argv, int start);

#endif // UNEBCC_ARGS_H
