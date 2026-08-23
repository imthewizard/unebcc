#ifndef UNEBCC_IR_BASIC_BLOCK_H
#define UNEBCC_IR_BASIC_BLOCK_H

#include "ir/instruction.h"

typedef struct IRBasicBlock {
	IRInstruction *instructions;
}IRBasicBlock;

void basic_block_print(IRBasicBlock *bb);

// Creates a new basic block
void basic_block_init(IRBasicBlock *bb);
// Frees a basic block
void basic_block_deinit(IRBasicBlock *bb);

#endif // UNEBCC_IR_BASIC_BLOCK_H
