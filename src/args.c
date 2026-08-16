#include <stdio.h>
#include <string.h>
#include "args.h"

ArgsContext args_parse(int argc, char **argv, int start)
{
	ArgsContext ctx = {
		.filename = NULL,
		.print_tokens = false,
		.print_ast = false,
		.print_ir = false,
	};

	for (int i = start; i < argc; i++) {
		if (strcmp(argv[i], "--token") == 0) {
			ctx.print_tokens = true;
			continue;
		}
		if (strcmp(argv[i], "--ast") == 0) {
			ctx.print_ast = true;
			continue;
		}
		if (strcmp(argv[i], "--ir") == 0) {
			ctx.print_ir = true;
			continue;
		}

		if (ctx.filename == NULL) {
			ctx.filename = argv[i];
			continue;
		}

		printf("Unknown option: %s\n", argv[i]);
	}

	return ctx;
}
