#include <stdio.h>
#include <assert.h>

#include "backend/x86_64/emitter.h"
#include "backend/x86_64/x86_64.h"
#include "utils/array.h"

static void print_setup(void);
static void print_end(void);
static void print_operand(const x86_64Operand *op);
static void emit_instruction(const x86_64Instruction *inst);

void x86_64_emit(const x86_64Program *prog)
{
	print_setup();
	for (int i = 0; i < array_length(prog->functions); i++) {
		x86_64Function *fn = &prog->functions[i];

		printf("%s:\n", fn->name);
		for (int j = 0; j < array_length(fn->instructions); j++) {
			x86_64Instruction *inst = &fn->instructions[j];
			emit_instruction(inst);
		}
	}
	print_end();
}

static void print_setup(void)
{
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
}

static void print_end(void)
{
	// TODO: linux only
	printf(".section .note.GNU-stack,\"\",@progbits");
}

static void print_operand(const x86_64Operand *op)
{
	switch (op->type) {
		case X86_64_IMMEDIATE:
			printf("%d", op->value.imm);
			break;
		case X86_64_REGISTER:
			printf("%s", x86_64_reg_to_str(op->value.reg));
			break;
		case X86_64_STACK:
			// spacing after operator
			if (op->value.stack < 0) {
				printf("dword ptr [rbp - %d]", -op->value.stack);
			} else {
				printf("dword ptr [rbp + %d]", op->value.stack);
			}
			break;

		case X86_64_PSEUDO: assert(0); // there should be no pseudos here
	}
}

static void emit_instruction(const x86_64Instruction *inst)
{
	switch (inst->mnemonic) {
		case X86_64_MOV:
			printf("mov ");
			print_operand(&inst->instruction.mov.dst);
			printf(", ");
			print_operand(&inst->instruction.mov.src);
			printf("\n");
			break;

		case X86_64_NEG:
			printf("neg ");
			print_operand(&inst->instruction.unary.src);
			printf("\n");
			break;
		case X86_64_NOT:
			printf("not ");
			print_operand(&inst->instruction.unary.src);
			printf("\n");
			break;

		case X86_64_RET:
			puts("ret");
			break;

		case X86_64_ALLOCATE_STACK:
			puts("push rbp");
			puts("mov rbp, rsp");
			printf("sub rsp, %d\n", inst->instruction.stack.value);
			break;
		case X86_64_DEALLOCATE_STACK:
			puts("mov rsp, rbp");
			puts("pop rbp");
			break;

		default: assert(0);
	}
}
