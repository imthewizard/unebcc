#ifndef UNEBCC_ARGS_H
#define UNEBCC_ARGS_H

#include <stdbool.h>

typedef struct ArgsContext {
	const char *filename;
	bool print_tokens;
	bool print_ast;
	bool print_ir;
}ArgsContext;

ArgsContext args_parse(int argc, char **argv, int start);

#endif // UNEBCC_ARGS_H
