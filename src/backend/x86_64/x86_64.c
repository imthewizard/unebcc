#include <stdio.h>
#include <assert.h>

#include "backend/x86_64/x86_64.h"

static const char *reg_to_str[] = {
	[X86_64_AX] = "AX",
	[X86_64_R10] = "R10",
};

static const char *mnemonic_to_str[] = {
	[X86_64_MOV] = "MOV",
	[X86_64_RET] = "RET",
	[X86_64_NEG] = "NEG",
	[X86_64_NOT] = "NOT",

	[X86_64_ALLOCATE_STACK] = "ALLOCATE_STACK",
	[X86_64_DEALLOCATE_STACK] = "DEALLOCATE_STACK",
};

static const char *optype_to_str[] = {
	[X86_64_IMMEDIATE] = "IMMEDIATE",
	[X86_64_REGISTER] = "REGISTER",
	[X86_64_PSEUDO] = "PSEUDO",
	[X86_64_STACK] = "STACK",
};

static void print_operand(const x86_64Operand *operand)
{
	printf("{%s ", optype_to_str[operand->type]);
	switch (operand->type) {
		case X86_64_IMMEDIATE: printf("%d", operand->value.imm); break;
		case X86_64_REGISTER: printf("%s", reg_to_str[operand->value.reg]); break;
		case X86_64_PSEUDO: printf("%d", operand->value.pseudo); break;
		case X86_64_STACK: printf("%d", operand->value.stack); break;

		default: assert(0);
	}
	printf("}");
}

void x86_64_print_inst(const x86_64Instruction *inst)
{
	printf("%s:\t", mnemonic_to_str[inst->mnemonic]);

	switch (inst->mnemonic) {
		case X86_64_MOV:
			print_operand(&inst->instruction.mov.dst);
			print_operand(&inst->instruction.mov.src);
			break;
		case X86_64_RET: break;
		case X86_64_NEG:
			print_operand(&inst->instruction.unary.src);
			break;
		case X86_64_NOT:
			print_operand(&inst->instruction.unary.src);
			break;

		case X86_64_ALLOCATE_STACK:
			printf("{ALLOC %d}", inst->instruction.stack.value);
			break;
		case X86_64_DEALLOCATE_STACK: break;

		default: assert(0);
	}

	printf("\n");
}
