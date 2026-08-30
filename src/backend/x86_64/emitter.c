#include <stdio.h>

#include "backend/x86_64/emitter.h"
#include "backend/x86_64/x86_64.h"
#include "utils/array.h"
#include "utils/debug.h"

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

		default: ASSERT(0, "invalid type: missing or failed regalloc");
	}
}

static void emit_instruction(const x86_64Instruction *inst)
{
	// Print instruction
	switch (inst->mnemonic) {
		case X86_64_MOV: printf("mov"); break;
		case X86_64_RET: printf("ret"); break;
		case X86_64_NEG: printf("neg"); break;
		case X86_64_NOT: printf("not"); break;

		case X86_64_ADD: printf("add"); break;
		case X86_64_SUB: printf("sub"); break;
		case X86_64_IMUL: printf("imul"); break;

		case X86_64_IDIV: printf("idiv"); break;

		case X86_64_CDQ: printf("cdq"); break;

		// Pseudo
		case X86_64_ALLOCATE_STACK:
		case X86_64_DEALLOCATE_STACK:
			break;

		default: UNIMPLEMENTED("Unhandled mnemonic case");
	}

	// Print operands
	switch (inst->mnemonic) {
		case X86_64_RET:
		case X86_64_CDQ:
			printf("\n");
			return;

		case X86_64_NEG:
		case X86_64_NOT:
		case X86_64_IDIV:
			printf(" ");
			print_operand(&inst->instruction.unary.src);
			printf("\n");
			return;

		case X86_64_MOV:
		case X86_64_ADD:
		case X86_64_SUB:
		case X86_64_IMUL:
			printf(" ");
			print_operand(&inst->instruction.binary.dst);
			printf(", ");
			print_operand(&inst->instruction.binary.src);
			printf("\n");
			return;

		// Pseudo
		case X86_64_ALLOCATE_STACK:
			puts("push rbp");
			puts("mov rbp, rsp");
			printf("sub rsp, %d\n", inst->instruction.unary.src.value.stack);
			return;
		case X86_64_DEALLOCATE_STACK:
			puts("mov rsp, rbp");
			puts("pop rbp");
			return;

		default: UNIMPLEMENTED("Unhandled mnemonic case");
	}

	// switch (inst->mnemonic) {
	// 	case X86_64_MOV:
	// 		printf("mov ");
	// 		print_operand(&inst->instruction.binary.dst);
	// 		printf(", ");
	// 		print_operand(&inst->instruction.binary.src);
	// 		printf("\n");
	// 		break;
	//
	// 	case X86_64_NEG:
	// 		printf("neg ");
	// 		print_operand(&inst->instruction.unary.src);
	// 		printf("\n");
	// 		break;
	// 	case X86_64_NOT:
	// 		printf("not ");
	// 		print_operand(&inst->instruction.unary.src);
	// 		printf("\n");
	// 		break;
	//
	// 	case X86_64_RET:
	// 		puts("ret");
	// 		break;
	//
	// 	case X86_64_ALLOCATE_STACK:
	// 		puts("push rbp");
	// 		puts("mov rbp, rsp");
	// 		printf("sub rsp, %d\n", inst->instruction.unary.src.value.stack);
	// 		break;
	// 	case X86_64_DEALLOCATE_STACK:
	// 		puts("mov rsp, rbp");
	// 		puts("pop rbp");
	// 		break;
	//
	// 	default: UNIMPLEMENTED("Unhandled mnemonic case");
	// }
}
