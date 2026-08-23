#ifndef UNEBCC_IR_FUNCTION_H
#define UNEBCC_IR_FUNCTION_H

#include "ir/basic_block.h"

typedef struct IRFunction {
	IRBasicBlock *basic_blocks;
}IRFunction;

void function_print(IRFunction *fn);

// Creates a new function
void function_init(IRFunction *fn);
// Frees a function
void function_deinit(IRFunction *fn);

#endif // UNEBCC_IR_FUNCTION_H
