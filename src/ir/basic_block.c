#include "ir/basic_block.h"
#include "utils/array.h"

void basic_block_print(IRBasicBlock *bb)
{
	for (int i = 0; i < array_length(bb->instructions); i++) {
		instruction_print(&bb->instructions[i]);
	}
}

void basic_block_init(IRBasicBlock *bb)
{
	bb->instructions = array_create(bb->instructions, 1);
}

void basic_block_deinit(IRBasicBlock *bb)
{
	if (bb->instructions == NULL) return;
	for (int i = 0; i < array_length(bb->instructions); i++) {
		IRInstruction *inst = &bb->instructions[i];
		if (inst->src1.type == IR_OPERAND_LABEL && inst->src1.label != NULL) {
			free(inst->src1.label);
		}
		if (inst->src2.type == IR_OPERAND_LABEL && inst->src2.label != NULL) {
			free(inst->src2.label);
		}
	}
	array_free(bb->instructions);
}
