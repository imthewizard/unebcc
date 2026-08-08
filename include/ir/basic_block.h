#ifndef UNEBCC_IR_BASIC_BLOCK_H
#define UNEBCC_IR_BASIC_BLOCK_H

#include "ir/instruction.h"

typedef struct IRBasicBlock {
	IRInstruction *instructions;
	int instruction_amount;
}IRBasicBlock;

void basic_block_print(IRBasicBlock *bb);

// Creates a new basic block
void basic_block_init(IRBasicBlock *bb);
// Frees a basic block
void basic_block_deinit(IRBasicBlock *bb);
// Pushes an instruction to the basic block
void basic_block_push(IRBasicBlock *bb, const IRInstruction *inst);

#endif // UNEBCC_IR_BASIC_BLOCK_H
