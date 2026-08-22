#include <stdio.h>
#include <stdlib.h>

#include "backend/x86_64/program.h"
#include "backend/x86_64/x86_64.h"

x86_64Program x86_64_program_init(void)
{
	return (x86_64Program){NULL, 0};
}

void x86_64_program_deinit(x86_64Program *prog)
{
	if (prog->functions == NULL) return;
	for (int i = 0; i < prog->function_amount; i++) {
		x86_64_function_deinit(&prog->functions[i]);
	}
	free(prog->functions);
}

void x86_64_program_print(const x86_64Program *prog)
{
	for (int i = 0; i < prog->function_amount; i++) {
		x86_64Function *fn = &prog->functions[i];

		for (int j = 0; j < fn->instruction_amount; j++) {
			x86_64_print_inst(&fn->instructions[j]);
		}
	}
}

void x86_64_program_push_fn(x86_64Program *prog, x86_64Function *fn)
{
	x86_64Function *tmp = realloc(prog->functions, sizeof(x86_64Function) * (prog->function_amount + 1));
	if (tmp == NULL) {
		puts("x86_64_push_function: realloc fail");
		exit(EXIT_FAILURE);
	}

	prog->functions = tmp;
	prog->functions[prog->function_amount++] = *fn;
}
