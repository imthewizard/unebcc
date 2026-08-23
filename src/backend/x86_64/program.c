#include "backend/x86_64/program.h"
#include "backend/x86_64/function.h"
#include "backend/x86_64/x86_64.h"
#include "utils/array.h"

x86_64Program x86_64_program_init(void)
{
	x86_64Program prog;
	prog.functions = array_create(prog.functions, 1);
	return prog;
}

void x86_64_program_deinit(x86_64Program *prog)
{
	if (prog->functions == NULL) return;
	for (int i = 0; i < array_length(prog->functions); i++) {
		x86_64_function_deinit(&prog->functions[i]);
	}
	array_free(prog->functions);

}

void x86_64_program_print(const x86_64Program *prog)
{
	for (int i = 0; i < array_length(prog->functions); i++) {
		x86_64Function *fn = &prog->functions[i];

		for (int j = 0; j < array_length(fn->instructions); j++) {
			x86_64_print_inst(&fn->instructions[j]);
		}
	}
}
