#ifndef UNEBCC_IR_FUNCTION_H
#define UNEBCC_IR_FUNCTION_H

#include "ir/basic_block.h"

typedef struct IRFunction {
	IRBasicBlock *basic_blocks;
	int basic_block_amount;
}IRFunction;

void function_print(IRFunction *fn);

// Creates a new function
void function_init(IRFunction *fn);
// Frees a function
void function_deinit(IRFunction *fn);
// Pushes a basic block to the function
void function_push(IRFunction *fn, const IRBasicBlock *bb);

#endif // UNEBCC_IR_FUNCTION_H
