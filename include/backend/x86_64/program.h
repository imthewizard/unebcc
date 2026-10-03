#ifndef UNEBCC_X86_64_PROGRAM_H
#define UNEBCC_X86_64_PROGRAM_H

#include "x86_64.h"


typedef struct x86_64Program {
	x86_64Instruction *instructions;
}x86_64Program;

// Creates a program
x86_64Program x86_64_program_init(void);
// Frees the program
void x86_64_program_deinit(x86_64Program *prog);
// Prints the program
void x86_64_program_print(const x86_64Program *prog);

#endif // UNEBCC_X86_64_PROGRAM_H
