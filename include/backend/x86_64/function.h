#ifndef UNEBCC_X86_64_FUNCTION_H
#define UNEBCC_X86_64_FUNCTION_H

#include "x86_64.h"

typedef struct x86_64Function {
	char *name;
	x86_64Instruction *instructions;
	int instruction_amount;
}x86_64Function;

// Creates a function
x86_64Function x86_64_function_init(const char *name);
// Deallocates a function
void x86_64_function_deinit(x86_64Function *fn);
// Pushes an instruction to the function
void x86_64_function_push_inst(x86_64Function *fn, x86_64Instruction *inst);


#endif // UNEBCC_X86_64_FUNCTION_H
