#include <stdio.h>
#include <string.h>

#include "ir/instruction.h"
#include "utils/debug.h"
#include "utils/stringmap.h"

void instruction_print(const IRInstruction *inst)
{
	if (inst->type == IR_LABEL) {
		printf("%s:\n", inst->src1.label);
		return;
	}

	if (inst->src1.type == IR_OPERAND_LABEL ||
		inst->src2.type == IR_OPERAND_LABEL) {
		switch (inst->type) {
			case IR_JUMP: printf("jump "); break;
			case IR_JUMP_ZERO: printf("jumpz "); break;
			case IR_JUMP_NOT_ZERO: printf("jumpnz "); break;

			default: UNIMPLEMENTED("Unhandled branching case");
		}
		if (inst->src1.type == IR_OPERAND_LABEL) {
			printf(":%s", inst->src1.label);
			printf("\n");
			return;
		} else if (inst->src1.type == IR_OPERAND_TEMP) {
			printf("tmp%d", inst->src1.value);
		} else {
			printf("%d", inst->src1.value);
		}
		if (inst->src2.type != IR_OPERAND_NULL) {
			if (inst->src2.type == IR_OPERAND_LABEL) {
				printf(", :%s", inst->src2.label);
			} else {
				ASSERT(0, "broken instruction");
			}
		}
		printf("\n");
		return;
	}

	if (inst->type == IR_COPY) {
		// Special case: copy
		if (inst->src2.type == IR_OPERAND_CONST) {
			printf("copy tmp%d, %d", inst->src1.value, inst->src2.value);
		} else {
			printf("copy tmp%d, tmp%d", inst->src1.value, inst->src2.value);
		}
		printf("\n");
		return;
	}
	if (inst->type == IR_RETURN) {
		// Special case: return
		printf("return ");
		if (inst->src1.type == IR_OPERAND_TEMP) {
			printf("tmp");
		}
		printf("%d", inst->src1.value);
		printf("\n");
	} else {
		printf("tmp%d = ", inst->dest_id);

		// Print instruction
		switch (inst->type) {
			case IR_BITWISE_NOT: printf("bnot "); break;
			case IR_BITWISE_AND: printf("band "); break;
			case IR_BITWISE_OR: printf("bor "); break;
			case IR_BITWISE_XOR: printf("bxor "); break;
			case IR_NEGATE: printf("not "); break;
			case IR_LOGICAL_NOT: printf("lognot "); break;

			case IR_ADD: printf("add "); break;
			case IR_SUBTRACT: printf("sub "); break;
			case IR_MULTIPLY: printf("mul "); break;
			case IR_DIVIDE: printf("div "); break;
			case IR_REMAINDER: printf("rem "); break;
			case IR_LEFT_SHIFT: printf("lshift "); break;
			case IR_RIGHT_SHIFT: printf("rshift "); break;
			case IR_LOGICAL_EQUAL: printf("equal "); break;
			case IR_LOGICAL_NOT_EQUAL: printf("nequal "); break;
			case IR_LOGICAL_LESS_THAN: printf("lthan "); break;
			case IR_LOGICAL_LESS_EQUAL: printf("lequal "); break;
			case IR_LOGICAL_GREATER_THAN: printf("gthan "); break;
			case IR_LOGICAL_GREATER_EQUAL: printf("gequal "); break;

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

void instruction_free(IRInstruction *inst)
{
	if (inst->src1.type == IR_OPERAND_LABEL && inst->src1.label != NULL) {
		free(inst->src1.label);
	}
	if (inst->src2.type == IR_OPERAND_LABEL && inst->src2.label != NULL) {
		free(inst->src2.label);
	}
}

IRTemporaryID instruction_generate_id(void)
{
	static IRTemporaryID counter = 0;
	return counter++;
}

char *instruction_generate_label(const char *name)
{
	static int counter = 0;

	int name_len = strlen(name);
	// HACK: we allocate space for 9 digits here, but it should be very hard to break this
	int buffer_size = name_len + 10;
	char *buffer = malloc(sizeof(char) * buffer_size);
	snprintf(buffer, buffer_size, "%s%d", name, counter++);
	return buffer;
}

IROperand ir_temp_from_variable(const char *identifier, StringMap *var_to_id)
{
	int *possible_id = string_map_get(var_to_id, identifier);
	if (possible_id != NULL) {
		return IR_OPERAND_CREATE(IR_OPERAND_TEMP, *possible_id);
	} else {
		int *new_id = malloc(sizeof(int));
		*new_id = instruction_generate_id();
		string_map_put(var_to_id, identifier, new_id);
		return IR_OPERAND_CREATE(IR_OPERAND_TEMP, *new_id);
	}
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
			return IR_INSTRUCTION_BINARY_CONST_TEMP(type, lhs->value, rhs->value)
		} else if (rhs->type == IR_OPERAND_CONST) {
			return IR_INSTRUCTION_BINARY_CONST_CONST(type, lhs->value, rhs->value)
		} else {
			ASSERT(0, "Illegal binary IR instruction: lhs const but rhs not temp or const");
		}
	}
}

IRInstruction ir_instruction_label(char *label_name)
{
	return IR_INSTRUCTION_LABEL(strdup(label_name));
}

IRInstruction ir_instruction_copy(const IROperand *lhs, const IROperand *rhs)
{
	ASSERT(lhs->type == IR_OPERAND_TEMP, "lhs not temp");

	if (rhs->type == IR_OPERAND_CONST) {
		return IR_INSTRUCTION_COPY_CONST(lhs->value, rhs->value);
	}
	if (rhs->type == IR_OPERAND_TEMP) {
		return IR_INSTRUCTION_COPY_TEMP(lhs->value, rhs->value);
	}

	UNIMPLEMENTED("unhandled rhs type");
}

IRInstruction ir_instruction_branch_always(IRInstructionType type, char *label_name)
{
	ASSERT(type == IR_JUMP, "type is not IR_JUMP"); // only jump should need this function for now
	return IR_INSTRUCTION_BRANCH_ALWAYS(type, strdup(label_name));
}

IRInstruction ir_instruction_branch_condition(IRInstructionType type, const IROperand *cond_op, char *label_name)
{
	return IR_INSTRUCTION_BRANCH_COND(type, cond_op->type, cond_op->value, strdup(label_name));
}
