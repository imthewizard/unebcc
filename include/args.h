#ifndef UNEBCC_ARGS_H
#define UNEBCC_ARGS_H

#include <stdbool.h>

#include "utils/stringmap.h"

typedef struct ArgInfo {
	const char *name;
	const char *description;
	bool requires_value;
	int index;
} ArgInfo;

typedef struct ArgsContext {
	// Map of arg to its value, gets filled after parsing
	StringMap user_args;

	// Map of arg to its info
	StringMap available_args;

	// Name of the file that will be compiled
	const char *filename;

	// If there was an invalid argument, this will be false
	bool ok;
}ArgsContext;

// Creates an ArgsContext
ArgsContext args_init(void);
// Frees an ArgsContext
void args_deinit(ArgsContext *ctx);
// Adds an argument and description. If provides_value is true, it also saves whatever comes after the argument (--arg=value)
void args_add_arg(ArgsContext *ctx, const char *arg, const char *desc, bool provides_value);
// Prints each argument and their description
void args_print_help(ArgsContext *ctx);
// Parses the passed args and modifies the passed context
void args_parse(ArgsContext *ctx, int argc, char **argv, int start);
// Returns true if arg exists
bool args_has(const ArgsContext *ctx, const char *arg);
// Returns true if the value of arg is equal to the expected value
bool args_cmp_value(const ArgsContext *ctx, const char *arg, const char *expected);

#endif // UNEBCC_ARGS_H
