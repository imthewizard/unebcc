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
	IR_STORE,
}IRInstructionType;

typedef struct IRInstruction {
	IRInstructionType type;
	IRTemporaryID dest_id;
	IROperand src1;
	IROperand src2;
}IRInstruction;

void instruction_print(const IRInstruction *inst);

// Returns a new unique temporary id
IRTemporaryID instruction_generate_id(void);

// Returns the instruction that represents a store into a temporary value
IRInstruction instruction_store_const(int value);
// Returns the instruction that represents a negation of a temporary into a temporary value
IRInstruction instruction_negate_temp(IRTemporaryID temp);
// Returns the instruction that represents a return
IRInstruction instruction_return(int value);

#endif // UNEBCC_IR_INSTRUCTION_H
