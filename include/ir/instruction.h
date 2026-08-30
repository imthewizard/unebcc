#ifndef UNEBCC_IR_INSTRUCTION_H
#define UNEBCC_IR_INSTRUCTION_H

typedef unsigned int IRTemporaryID;

typedef enum IROperandType {
	IR_OPERAND_NULL,
	IR_OPERAND_TEMP,
	IR_OPERAND_CONST,
}IROperandType;

typedef struct IROperand {
	IROperandType type;
	int value;
}IROperand;

typedef enum IRInstructionType {
	IR_RETURN,

	IR_BITWISE_NOT,
	IR_NEGATE,

	IR_ADD,
	IR_SUBTRACT,
	IR_MULTIPLY,
	IR_DIVIDE,
	IR_REMAINDER,
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

// Creates a new unary instruction
IRInstruction ir_instruction_unary(IRInstructionType type, const IROperand *op);
// Creates a new binary instruction
IRInstruction ir_instruction_binary(IRInstructionType type, const IROperand *lhs, const IROperand *rhs);

#define IR_OPERAND_CREATE(optype, val) \
	(IROperand){.type = (optype), .value = (val)}

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

#define IR_INSTRUCTION_RETURN(temp_id) \
	_IR_INSTRUCTION_NO_DST((IR_RETURN), (IR_OPERAND_TEMP), (temp_id))

#define IR_INSTRUCTION_UNARY_TEMP(instruction, temp_id) \
	_IR_INSTRUCTION_UNARY((instruction), (IR_OPERAND_TEMP), (temp_id))
#define IR_INSTRUCTION_UNARY_CONST(instruction, const_val) \
	_IR_INSTRUCTION_UNARY((instruction), (IR_OPERAND_CONST), (const_val))

#define IR_INSTRUCTION_BINARY_TEMP_TEMP(instruction, temp1, temp2) \
	_IR_INSTRUCTION_BINARY((instruction), (IR_OPERAND_TEMP), (temp1), (IR_OPERAND_TEMP), (temp2))
#define IR_INSTRUCTION_BINARY_TEMP_CONST(instruction, temp1, const_val) \
	_IR_INSTRUCTION_BINARY((instruction), (IR_OPERAND_TEMP), (temp1), (IR_OPERAND_CONST), (const_val))
#define IR_INSTRUCTION_BINARY_CONST_CONST(instruction, const1, const2) \
	_IR_INSTRUCTION_BINARY((instruction), (IR_OPERAND_CONST), (const1), (IR_OPERAND_CONST), (const2))


#endif // UNEBCC_IR_INSTRUCTION_H
