#ifndef UNEBCC_X86_64_PROGRAM_H
#define UNEBCC_X86_64_PROGRAM_H

#include "function.h"

typedef struct x86_64Program {
	x86_64Function *functions;
	int function_amount;
}x86_64Program;

// Creates a program
x86_64Program x86_64_program_init(void);
// Frees the program
void x86_64_program_deinit(x86_64Program *prog);
// Prints the program
void x86_64_program_print(const x86_64Program *prog);
// Pushes a function to the program
void x86_64_program_push_fn(x86_64Program *prog, x86_64Function *fn);

#endif // UNEBCC_X86_64_PROGRAM_H
