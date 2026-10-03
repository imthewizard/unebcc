#include "backend/x86_64/program.h"
#include "backend/x86_64/x86_64.h"
#include "utils/array.h"

x86_64Program x86_64_program_init(void)
{
	x86_64Program prog;
	prog.instructions = array_create(prog.instructions, 1);
	return prog;
}

void x86_64_program_deinit(x86_64Program *prog)
{
	array_free(prog->instructions);
}

void x86_64_program_print(const x86_64Program *prog)
{
	for (int i = 0; i < array_length(prog->instructions); i++) {
		x86_64_print_inst(&prog->instructions[i]);
	}
}
