#include <stdio.h>

#include "ir/instruction.h"
#include "utils/debug.h"

void instruction_print(const IRInstruction *inst)
{
	switch(inst->type){
		case IR_STORE:
			printf("tmp%d = const %d", inst->dest_id, inst->src1.value);
			break;

		case IR_RETURN:
			printf("return tmp%d", inst->src1.value);
			break;

		case IR_BITWISE_NOT:
			printf("tmp%d = not tmp%d", inst->dest_id, inst->src1.value);
			break;
		case IR_NEGATE:
			printf("tmp%d = negate tmp%d", inst->dest_id, inst->src1.value);
			break;

		case IR_ADD:
			printf("tmp%d = add tmp%d, tmp%d", inst->dest_id, inst->src1.value, inst->src2.value);
			break;
		case IR_SUBTRACT:
			printf("tmp%d = subtract tmp%d, tmp%d", inst->dest_id, inst->src1.value, inst->src2.value);
			break;
		case IR_MULTIPLY:
			printf("tmp%d = multiply tmp%d, tmp%d", inst->dest_id, inst->src1.value, inst->src2.value);
			break;
		case IR_DIVIDE:
			printf("tmp%d = divide tmp%d, tmp%d", inst->dest_id, inst->src1.value, inst->src2.value);
			break;
		case IR_REMAINDER:
			printf("tmp%d = remainder tmp%d, tmp%d", inst->dest_id, inst->src1.value, inst->src2.value);
			break;

		default: UNIMPLEMENTED("Unhandled instruction type case");
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

IRInstruction instruction_add(IRTemporaryID lhs, IRTemporaryID rhs)
{
	return (IRInstruction){
		.type = IR_ADD,
		.dest_id = instruction_generate_id(),
		.src1 = {.type = IR_OPERAND_TEMP, .value = lhs},
		.src2 = {.type = IR_OPERAND_TEMP, .value = rhs},
	};
}

IRInstruction instruction_sub(IRTemporaryID lhs, IRTemporaryID rhs)
{
	return (IRInstruction){
		.type = IR_SUBTRACT,
		.dest_id = instruction_generate_id(),
		.src1 = {.type = IR_OPERAND_TEMP, .value = lhs},
		.src2 = {.type = IR_OPERAND_TEMP, .value = rhs},
	};
}

IRInstruction instruction_mul(IRTemporaryID lhs, IRTemporaryID rhs)
{
	return (IRInstruction){
		.type = IR_MULTIPLY,
		.dest_id = instruction_generate_id(),
		.src1 = {.type = IR_OPERAND_TEMP, .value = lhs},
		.src2 = {.type = IR_OPERAND_TEMP, .value = rhs},
	};
}

IRInstruction instruction_div(IRTemporaryID lhs, IRTemporaryID rhs)
{
	return (IRInstruction){
		.type = IR_DIVIDE,
		.dest_id = instruction_generate_id(),
		.src1 = {.type = IR_OPERAND_TEMP, .value = lhs},
		.src2 = {.type = IR_OPERAND_TEMP, .value = rhs},
	};
}

IRInstruction instruction_rem(IRTemporaryID lhs, IRTemporaryID rhs)
{
	return (IRInstruction){
		.type = IR_REMAINDER,
		.dest_id = instruction_generate_id(),
		.src1 = {.type = IR_OPERAND_TEMP, .value = lhs},
		.src2 = {.type = IR_OPERAND_TEMP, .value = rhs},
	};
}
