#ifndef UNEBCC_X86_64_H
#define UNEBCC_X86_64_H

#include "ir/instruction.h"

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
		} binary;

		struct {
			x86_64Operand src;
		} unary;
	} instruction;
}x86_64Instruction;

// Prints the instruction
void x86_64_print_inst(const x86_64Instruction *inst);
// Converts a register enum value to a string
const char *x86_64_reg_to_str(x86_64Registers reg);
// Converts an IR operand to a x86_64 operand
x86_64Operand x86_64_ir_operand(const IROperand *ir_op);

#define X64_OPERAND_NULL \
	(x86_64Operand){.type = X86_64_NULL}
#define X64_OPERAND_IMM(val) \
	(x86_64Operand){.type = X86_64_IMMEDIATE, .value.imm = (val)}
#define X64_OPERAND_REG(val) \
	(x86_64Operand){.type = X86_64_REGISTER, .value.reg = (val)}
#define X64_OPERAND_PSEUDO(val) \
	(x86_64Operand){.type = X86_64_PSEUDO, .value.pseudo = (val)}
#define X64_OPERAND_STACK(val) \
	(x86_64Operand){.type = X86_64_STACK, .value.stack = (val)}

#define X64_INSTRUCTION_NO_OPS(mnem) \
	(x86_64Instruction){ \
		.mnemonic = (mnem), \
	}
#define X64_INSTRUCTION_BINARY(mnem, dst_op, src_op) \
	(x86_64Instruction){ \
		.mnemonic = (mnem), \
		.instruction.binary.dst = (dst_op), \
		.instruction.binary.src = (src_op), \
	}
#define X64_INSTRUCTION_UNARY(mnem, src_op) \
	(x86_64Instruction){ \
		.mnemonic = (mnem), \
		.instruction.unary.src = (src_op), \
	}
#define X64_INSTRUCTION_STACK(mnem, stack) \
	(x86_64Instruction){ \
		.mnemonic = (mnem), \
		.instruction.unary.src = X64_OPERAND_STACK((stack)) \
	}

#endif // UNEBCC_X86_64_H
