#include <stdio.h>
#include <string.h>
#include "args.h"

ArgsContext args_parse(int argc, char **argv, int start)
{
	ArgsContext ctx = {
		.filename = NULL,
		.print_lexer = false,
		.print_parser = false,
		.print_ir = false,
		.print_machine_ir = false,
		.print_asm = false,

		.ok = true,
	};

	for (int i = start; i < argc; i++) {
		if (strcmp(argv[i], "--lexer") == 0) {
			ctx.print_lexer = true;
			continue;
		}
		if (strcmp(argv[i], "--parser") == 0) {
			ctx.print_parser = true;
			continue;
		}
		if (strcmp(argv[i], "--ir") == 0) {
			ctx.print_ir = true;
			continue;
		}
		if (strcmp(argv[i], "--machine-ir") == 0) {
			ctx.print_machine_ir = true;
			continue;
		}
		if (strcmp(argv[i], "--print-asm") == 0) {
			ctx.print_asm = true;
			continue;
		}

		if (ctx.filename == NULL) {
			ctx.filename = argv[i];
			continue;
		}

		printf("Unknown option: %s\n", argv[i]);
		ctx.ok = false;
	}

	return ctx;
}
