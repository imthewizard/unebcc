#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backend/x86_64/function.h"
#include "backend/x86_64/x86_64.h"

x86_64Function x86_64_function_init(const char *name)
{
	x86_64Function fn;
	fn.instructions = NULL;
	fn.instruction_amount = 0;

	int name_len = strlen(name);
	fn.name = malloc(sizeof(char) * name_len);
	strcpy(fn.name, name);

	return fn;
}

void x86_64_function_deinit(x86_64Function *fn)
{
	if (fn->name != NULL) {
		free(fn->name);
	}
	if (fn->instructions != NULL) {
		free(fn->instructions);
	}
}

void x86_64_function_push_inst(x86_64Function *fn, x86_64Instruction *inst)
{
	x86_64Instruction *tmp = realloc(fn->instructions, sizeof(x86_64Instruction) * (fn->instruction_amount + 1));
	if (tmp == NULL) {
		puts("x86_64_push_instruction: realloc fail");
		exit(EXIT_FAILURE);
	}

	fn->instructions = tmp;
	fn->instructions[fn->instruction_amount++] = *inst;
}
