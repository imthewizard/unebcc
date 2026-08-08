#include <stdio.h>
#include <assert.h>

#include "ir/instruction.h"

void instruction_print(const IRInstruction *inst)
{
	switch(inst->type){
		case IR_RETURN:
			printf("return tmp%d", inst->src1.value);
			break;
		case IR_BITWISE_NOT:
			printf("tmp%d = ~ tmp%d", inst->dest_id, inst->src1.value);
			break;
		case IR_NEGATE:
			printf("tmp%d = - tmp%d", inst->dest_id, inst->src1.value);
			break;
		case IR_STORE:
			printf("tmp%d = const %d", inst->dest_id, inst->src1.value);
			break;

		default:
			assert(0); // unimplemented
	}
	printf("\n");
}

IRTemporaryID instruction_generate_id(void)
{
	static IRTemporaryID counter = 0;
	return counter++;
}

IRInstruction instruction_store_const(int value)
{
	return (IRInstruction){
		.type = IR_STORE,
		.dest_id = instruction_generate_id(),
		.src1 = {.type = IR_OPERAND_CONST, .value = value},
		.src2 = {.type = IR_OPERAND_NULL},
	};
}

IRInstruction instruction_negate_temp(IRTemporaryID temp)
{
	return (IRInstruction){
		.type = IR_NEGATE,
		.dest_id = instruction_generate_id(),
		.src1 = {.type = IR_OPERAND_TEMP, .value = temp},
		.src2 = {.type = IR_OPERAND_NULL},
	};
}

IRInstruction instruction_return(int value)
{
	return (IRInstruction){
		.type = IR_RETURN,
		.src1 = {.type = IR_OPERAND_TEMP, .value = value},
	};
}
