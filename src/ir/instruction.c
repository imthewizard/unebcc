#include <stdio.h>

#include "ir/instruction.h"
#include "utils/debug.h"

void instruction_print(const IRInstruction *inst)
{
	if (inst->type == IR_RETURN) {
		// Special case: return
		printf("return ");
		if (inst->src1.type == IR_OPERAND_TEMP) {
			printf("tmp");
		}
		printf("%d", inst->src1.value);
	} else {
		printf("tmp%d = ", inst->dest_id);

		// Print instruction
		switch (inst->type) {
			case IR_BITWISE_NOT: printf("bnot "); break;
			case IR_NEGATE: printf("not "); break;

			case IR_ADD: printf("add "); break;
			case IR_SUBTRACT: printf("sub "); break;
			case IR_MULTIPLY: printf("mul "); break;
			case IR_DIVIDE: printf("div "); break;
			case IR_REMAINDER: printf("rem "); break;

			default: UNIMPLEMENTED("Unhandled case in instruction_print");
		}

		// Print src1
		if (inst->src1.type == IR_OPERAND_TEMP) {
			printf("tmp");
		}
		printf("%d", inst->src1.value);

		// Print src2 if it exists
		if (inst->src2.type != IR_OPERAND_NULL) {
			printf(", ");
			if (inst->src2.type == IR_OPERAND_TEMP) {
				printf("tmp");
			}
			printf("%d", inst->src2.value);
		}

		printf("\n"); // end
	}
}

IRTemporaryID instruction_generate_id(void)
{
	static IRTemporaryID counter = 0;
	return counter++;
}
IRInstruction ir_instruction_unary(IRInstructionType type, const IROperand *op)
{
	if (op->type == IR_OPERAND_TEMP) {
		return IR_INSTRUCTION_UNARY_TEMP(type, op->value);
	} else if (op->type == IR_OPERAND_CONST) {
		return IR_INSTRUCTION_UNARY_CONST(type, op->value);
	} else {
		ASSERT(0, "Illegal unary IR instruction: not temp or const");
	}
}

IRInstruction ir_instruction_binary(IRInstructionType type, const IROperand *lhs, const IROperand *rhs)
{
	ASSERT(((lhs->type == IR_OPERAND_TEMP) || (lhs->type == IR_OPERAND_CONST)), "Illegal binary IR instruction: lhs not temp or const");

	if (lhs->type == IR_OPERAND_TEMP) {
		if (rhs->type == IR_OPERAND_TEMP) {
			return IR_INSTRUCTION_BINARY_TEMP_TEMP(type, lhs->value, rhs->value)
		} else if (rhs->type == IR_OPERAND_CONST) {
			return IR_INSTRUCTION_BINARY_TEMP_CONST(type, lhs->value, rhs->value)
		} else {
			ASSERT(0, "Illegal binary IR instruction: lhs temp but rhs not temp or const");
		}
	} else {
		if (rhs->type == IR_OPERAND_TEMP) {
			return IR_INSTRUCTION_BINARY_TEMP_CONST(type, rhs->value, lhs->value)
		} else if (rhs->type == IR_OPERAND_CONST) {
			return IR_INSTRUCTION_BINARY_CONST_CONST(type, lhs->value, rhs->value)
		} else {
			ASSERT(0, "Illegal binary IR instruction: lhs const but rhs not temp or const");
		}
	}
}
