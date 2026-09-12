#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"

#include "driver/driver.h"

int main(int argc, char **argv)
{
	ArgsContext ctx = args_init();

	args_add_arg(&ctx, "lex", "Runs the lexer and stops before parsing", false);
	args_add_arg(&ctx, "parse", "Runs the parser and stops before assembly generation", false);
	args_add_arg(&ctx, "codegen", "Lexes, parses and generates assembly, but doesn't save it to a file", false);
	args_add_arg(&ctx, "print", "Expects 'val' to be lexer/parser/ir/machine-ir/asm. Will print the output to stdout", true);

	if (argc < 2){
		printf("%s [FILE] [FLAGS]\n", argv[0]);
		args_print_help(&ctx);
		args_deinit(&ctx);
		return EXIT_FAILURE;
	}

	args_parse(&ctx, argc, argv, 1);
	if (ctx.filename == NULL) {
		puts("Missing file");
		args_deinit(&ctx);
		return EXIT_FAILURE;
	} else if (ctx.ok == false) {
		args_deinit(&ctx);
		return EXIT_FAILURE;
	}

	driver_start(&ctx);

	args_deinit(&ctx);
	return EXIT_SUCCESS;
}
