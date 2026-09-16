#include <stdio.h>

#include "ir/instruction.h"

#include "backend/x86_64/x86_64.h"
#include "utils/debug.h"

static const char *reg32_to_str[] = {
	[X86_64_AX]  = "eax",
	[X86_64_CX] = "ecx",
	[X86_64_DX]  = "edx",
	[X86_64_R10] = "r10d",
	[X86_64_R11] = "r11d",
};

static const char *reg8_to_str[] = {
	[X86_64_AX]  = "al",
	[X86_64_CX] = "cl",
	[X86_64_DX]  = "dl",
	[X86_64_R10] = "r10b",
	[X86_64_R11] = "r11b",
};

static const char *mnemonic_to_str[] = {
	[X86_64_MOV]   = "mov",
	[X86_64_RET]   = "ret",
	[X86_64_NEG]   = "neg",
	[X86_64_NOT]   = "not",
	[X86_64_JMP]   = "jmp",
	[X86_64_JE]    = "je",
	[X86_64_JNE]   = "jne",
	[X86_64_JG]    = "jg",
	[X86_64_JGE]   = "jge",
	[X86_64_JL]    = "jl",
	[X86_64_JLE]   = "jle",
	[X86_64_SETE]  = "sete",
	[X86_64_SETNE] = "setne",
	[X86_64_SETG]  = "setg",
	[X86_64_SETGE] = "setge",
	[X86_64_SETL]  = "setl",
	[X86_64_SETLE] = "setle",

	[X86_64_ADD]  = "add",
	[X86_64_SUB]  = "sub",
	[X86_64_IMUL] = "imul",
	[X86_64_AND]  = "and",
	[X86_64_OR]   = "or",
	[X86_64_XOR]  = "xor",
	[X86_64_SHL]  = "shl",
	[X86_64_SAR]  = "sar",
	[X86_64_CMP]  = "cmp",

	[X86_64_IDIV] = "idiv",

	[X86_64_CDQ] = "cdq",

	[X86_64_ALLOCATE_STACK] = "ALLOCATE_STACK",
	[X86_64_DEALLOCATE_STACK] = "DEALLOCATE_STACK",
	[X86_64_DEFINE_LABEL] = "DEFINE_LABEL",
};

static const char *optype_to_str[] = {
	[X86_64_IMMEDIATE] = "IMMEDIATE",
	[X86_64_REGISTER] = "REGISTER",
	[X86_64_PSEUDO] = "PSEUDO",
	[X86_64_STACK] = "STACK",
	[X86_64_LABEL] = "LABEL",
};

static void print_operand(const x86_64Operand *operand, int register_size)
{
	printf("{%s ", optype_to_str[operand->type]);
	switch (operand->type) {
		case X86_64_IMMEDIATE: printf("%d", operand->value.imm); break;
		// case X86_64_REGISTER: printf("%s", reg_to_str[operand->value.reg]); break;
		case X86_64_REGISTER: printf("%s", x86_64_reg_to_str(operand->value.reg, register_size)); break;
		case X86_64_PSEUDO: printf("%d", operand->value.pseudo); break;
		case X86_64_STACK: printf("%d", operand->value.stack); break;
		case X86_64_LABEL: printf("%s", operand->value.label); break;

		default: UNIMPLEMENTED("Unhandled type case");
	}
	printf("}");
}

void x86_64_print_inst(const x86_64Instruction *inst)
{
	printf("%s:\t", mnemonic_to_str[inst->mnemonic]);

	switch (inst->mnemonic) {
		case X86_64_ADD:
		case X86_64_SUB:
		case X86_64_IMUL:
		case X86_64_MOV:
		case X86_64_AND:
		case X86_64_OR:
		case X86_64_XOR:
		case X86_64_CMP:
			print_operand(&inst->instruction.binary.dst, 4);
			print_operand(&inst->instruction.binary.src, 4);
			break;

		case X86_64_SHL:
		case X86_64_SAR:
			print_operand(&inst->instruction.binary.dst, 1);
			print_operand(&inst->instruction.binary.src, 1);
			break;

		case X86_64_SETE:
		case X86_64_SETNE:
		case X86_64_SETG:
		case X86_64_SETGE:
		case X86_64_SETL:
		case X86_64_SETLE:
			print_operand(&inst->instruction.unary.src, 1);
			break;

		case X86_64_CDQ:
		case X86_64_RET: break;

		case X86_64_IDIV:
		case X86_64_NEG:
		case X86_64_NOT:
		case X86_64_JMP:
		case X86_64_JE:
		case X86_64_JNE:
		case X86_64_JG:
		case X86_64_JGE:
		case X86_64_JL:
		case X86_64_JLE:
			print_operand(&inst->instruction.unary.src, 4);
			break;

		case X86_64_ALLOCATE_STACK:
			printf("{ALLOC %d}", inst->instruction.unary.src.value.stack);
			break;
		case X86_64_DEALLOCATE_STACK: break;
		case X86_64_DEFINE_LABEL:
			printf("{LABEL %s}", inst->instruction.unary.src.value.label);
			break;

		default: UNIMPLEMENTED("Unhandled mnemonic case");
	}

	printf("\n");
}

x86_64Operand x86_64_ir_operand(const IROperand *ir_op)
{
	switch (ir_op->type) {
		case IR_OPERAND_TEMP:
			return X64_OPERAND_PSEUDO(ir_op->value);
		case IR_OPERAND_CONST:
			return X64_OPERAND_IMM(ir_op->value);
		default: UNIMPLEMENTED("Unhandled ir_op type");
	}
}

const char *x86_64_reg_to_str(x86_64Registers reg, int byte_amount)
{
	switch (byte_amount) {
		case 4: return reg32_to_str[reg];

		case 1: return reg8_to_str[reg];

		default: UNIMPLEMENTED("unhandled byte amount");
	}
}

const char *x86_64_mnemonic_to_str(x86_64Mnemonics mnemonic)
{
	return mnemonic_to_str[mnemonic];
}
