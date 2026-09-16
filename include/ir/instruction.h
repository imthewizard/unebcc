#ifndef UNEBCC_IR_INSTRUCTION_H
#define UNEBCC_IR_INSTRUCTION_H

#include <stdlib.h> // NULL

typedef unsigned int IRTemporaryID;

typedef enum IROperandType {
	IR_OPERAND_NULL,
	IR_OPERAND_TEMP,
	IR_OPERAND_CONST,
	IR_OPERAND_LABEL,
}IROperandType;

typedef struct IROperand {
	IROperandType type;
	int value;
	char *label;
}IROperand;

typedef enum IRInstructionType {
	IR_LABEL,

	IR_RETURN,

	IR_BITWISE_NOT,
	IR_NEGATE,
	IR_LOGICAL_NOT,
	IR_COPY,
	IR_JUMP,
	IR_JUMP_ZERO,
	IR_JUMP_NOT_ZERO,

	IR_ADD,
	IR_SUBTRACT,
	IR_MULTIPLY,
	IR_DIVIDE,
	IR_REMAINDER,
	IR_BITWISE_AND,
	IR_BITWISE_OR,
	IR_BITWISE_XOR,
	IR_LEFT_SHIFT,
	IR_RIGHT_SHIFT,
	IR_LOGICAL_EQUAL,
	IR_LOGICAL_NOT_EQUAL,
	IR_LOGICAL_LESS_THAN,
	IR_LOGICAL_LESS_EQUAL,
	IR_LOGICAL_GREATER_THAN,
	IR_LOGICAL_GREATER_EQUAL,
}IRInstructionType;

typedef struct IRInstruction {
	IRInstructionType type;
	IRTemporaryID dest_id;
	IROperand src1;
	IROperand src2;
}IRInstruction;

// Prints the instruction
void instruction_print(const IRInstruction *inst);

// Returns a new unique temporary id
IRTemporaryID instruction_generate_id(void);
// Returns the label with a unique id, must be freed later
char *instruction_generate_label(const char *name);

// Creates a new unary instruction
IRInstruction ir_instruction_unary(IRInstructionType type, const IROperand *op);
// Creates a new binary instruction
IRInstruction ir_instruction_binary(IRInstructionType type, const IROperand *lhs, const IROperand *rhs);
// Creates a new label instruction
IRInstruction ir_instruction_label(char *label_name);
// Creates a new branch instruction that does not require a condition. label_name is duplicated and can be freed after calling this
IRInstruction ir_instruction_branch_always(IRInstructionType type, char *label_name);
// Creates a new branch instruction that requires a condition. label_name is duplicated and can be freed after calling this
IRInstruction ir_instruction_branch_condition(IRInstructionType type, const IROperand *cond_op, char *label_name);

#define IR_OPERAND_CREATE(optype, val) \
	(IROperand){.type = (optype), .value = (val), .label = NULL}
#define IR_OPERAND_CREATE_TEMP() \
	(IROperand){.type = IR_OPERAND_TEMP, .value = instruction_generate_id(), .label = NULL}

#define _IR_INSTRUCTION_NO_DST(instruction, src1_type, val) \
	(IRInstruction){ \
		.type = (instruction), \
		.src1 = {.type = (src1_type), .value = (val)}, \
		.src2 = {.type = IR_OPERAND_NULL}, \
	};
#define _IR_INSTRUCTION_UNARY(instruction, src1_type, val) \
	(IRInstruction){ \
		.type = (instruction), \
		.dest_id = instruction_generate_id(), \
		.src1 = {.type = (src1_type), .value = (val)}, \
		.src2 = {.type = IR_OPERAND_NULL}, \
	};
#define _IR_INSTRUCTION_BINARY(instruction, src1_type, val1, src2_type, val2) \
	(IRInstruction){ \
		.type = (instruction), \
		.dest_id = instruction_generate_id(), \
		.src1 = {.type = (src1_type), .value = (val1)}, \
		.src2 = {.type = (src2_type), .value = (val2)}, \
	};

#define IR_INSTRUCTION_LABEL(name) \
	(IRInstruction){ \
		.type = IR_LABEL, \
		.src1 = {.type = IR_OPERAND_LABEL, .label = (name)}, \
	};
#define IR_INSTRUCTION_RETURN(operand) \
	_IR_INSTRUCTION_NO_DST((IR_RETURN), (operand).type, (operand).value)
#define IR_INSTRUCTION_COPY(temp_id, constant) \
	(IRInstruction){ \
		.type = IR_COPY, \
		.src1 = {.type = IR_OPERAND_TEMP, .value = (temp_id)}, \
		.src2 = {.type = IR_OPERAND_CONST, .value = (constant)}, \
	};

#define IR_INSTRUCTION_UNARY_TEMP(instruction, temp_id) \
	_IR_INSTRUCTION_UNARY((instruction), (IR_OPERAND_TEMP), (temp_id))
#define IR_INSTRUCTION_UNARY_CONST(instruction, const_val) \
	_IR_INSTRUCTION_UNARY((instruction), (IR_OPERAND_CONST), (const_val))

#define IR_INSTRUCTION_BINARY_TEMP_TEMP(instruction, temp1, temp2) \
	_IR_INSTRUCTION_BINARY((instruction), (IR_OPERAND_TEMP), (temp1), (IR_OPERAND_TEMP), (temp2))
#define IR_INSTRUCTION_BINARY_TEMP_CONST(instruction, temp1, const_val) \
	_IR_INSTRUCTION_BINARY((instruction), (IR_OPERAND_TEMP), (temp1), (IR_OPERAND_CONST), (const_val))
#define IR_INSTRUCTION_BINARY_CONST_TEMP(instruction, const_val, temp1) \
	_IR_INSTRUCTION_BINARY((instruction), (IR_OPERAND_CONST), (const_val), (IR_OPERAND_TEMP), (temp1))
#define IR_INSTRUCTION_BINARY_CONST_CONST(instruction, const1, const2) \
	_IR_INSTRUCTION_BINARY((instruction), (IR_OPERAND_CONST), (const1), (IR_OPERAND_CONST), (const2))
#define IR_INSTRUCTION_BRANCH_ALWAYS(instruction, label_name) \
	(IRInstruction){ \
		.type = (instruction), \
		.src1 = {.type = (IR_OPERAND_LABEL), .label = (label_name)}, \
	};
#define IR_INSTRUCTION_BRANCH_COND(instruction, cond_type, cond_val, label_name) \
	(IRInstruction){ \
		.type = (instruction), \
		.src1 = {.type = (cond_type), .value = (cond_val)}, \
		.src2 = {.type = (IR_OPERAND_LABEL), .label = (label_name)}, \
	};


#endif // UNEBCC_IR_INSTRUCTION_H
