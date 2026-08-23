#ifndef UNEBCC_X86_64_H
#define UNEBCC_X86_64_H

typedef enum x86_64Registers {
	X86_64_AX,
	X86_64_R10,
}x86_64Registers;

typedef enum x86_64Mnemonics {
	X86_64_MOV,
	X86_64_RET,
	X86_64_NEG,
	X86_64_NOT,

	// Pseudo
	X86_64_ALLOCATE_STACK,
	X86_64_DEALLOCATE_STACK,
}x86_64Mnemonics;

typedef enum x86_64OperandType {
	X86_64_IMMEDIATE,
	X86_64_REGISTER,
	X86_64_PSEUDO,
	X86_64_STACK,
}x86_64OperandType;

typedef struct x86_64Operand {
	x86_64OperandType type;
	union {
		int imm;
		x86_64Registers reg;
		int pseudo;
		int stack;
	} value;
}x86_64Operand;

typedef struct x86_64Instruction {
	x86_64Mnemonics mnemonic;

	union {
		struct {
			x86_64Operand dst;
			x86_64Operand src;
		} mov;

		struct {
			x86_64Operand src;
		} unary;

		struct {
			int value;
		} stack;
	} instruction;
}x86_64Instruction;

// Prints the instruction
void x86_64_print_inst(const x86_64Instruction *inst);
// Converts a register enum value to a string
const char *x86_64_reg_to_str(x86_64Registers reg);

#endif // UNEBCC_X86_64_H
