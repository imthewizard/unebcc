#include <stdio.h>

#include "backend/x86_64/emitter.h"
#include "backend/x86_64/x86_64.h"
#include "utils/array.h"
#include "utils/debug.h"
#include "utils/os.h"

static void print_setup(FILE *file);
static void print_end(FILE *file);
static void print_operand(FILE *file, const x86_64Operand *op);
static void emit_instruction(FILE *file, const x86_64Instruction *inst);

void x86_64_emit(FILE *file, const x86_64Program *prog)
{
	print_setup(file);
	for (int i = 0; i < array_length(prog->functions); i++) {
		x86_64Function *fn = &prog->functions[i];

		fprintf(file, "%s:\n", fn->name);
		for (int j = 0; j < array_length(fn->instructions); j++) {
			x86_64Instruction *inst = &fn->instructions[j];
			emit_instruction(file, inst);
		}
	}
	print_end(file);
}

static void print_setup(FILE *file)
{
	fprintf(file, ".intel_syntax noprefix\n");
	fprintf(file, ".globl main\n");
}

static void print_end(FILE *file)
{
	#ifdef UNEBCC_LINUX
	fprintf(file, ".section .note.GNU-stack,\"\",@progbits\n");
	#endif
}

static void print_operand(FILE *file, const x86_64Operand *op)
{
	switch (op->type) {
		case X86_64_IMMEDIATE:
			fprintf(file, "%d", op->value.imm);
			break;
		case X86_64_REGISTER:
			fprintf(file, "%s", x86_64_reg_to_str(op->value.reg));
			break;
		case X86_64_STACK:
			// spacing after operator
			if (op->value.stack < 0) {
				fprintf(file, "dword ptr [rbp - %d]", -op->value.stack);
			} else {
				fprintf(file, "dword ptr [rbp + %d]", op->value.stack);
			}
			break;

		default: ASSERT(0, "invalid type: missing or failed regalloc");
	}
}

static void emit_instruction(FILE *file, const x86_64Instruction *inst)
{
	// Print instruction
	switch (inst->mnemonic) {
		case X86_64_MOV: fprintf(file, "mov"); break;
		case X86_64_RET: fprintf(file, "ret"); break;
		case X86_64_NEG: fprintf(file, "neg"); break;
		case X86_64_NOT: fprintf(file, "not"); break;

		case X86_64_ADD: fprintf(file, "add"); break;
		case X86_64_SUB: fprintf(file, "sub"); break;
		case X86_64_IMUL: fprintf(file, "imul"); break;
		case X86_64_AND: fprintf(file, "and"); break;
		case X86_64_OR: fprintf(file, "or"); break;
		case X86_64_XOR: fprintf(file, "xor"); break;
		case X86_64_SHL: fprintf(file, "shl"); break;
		case X86_64_SAR: fprintf(file, "sar"); break;

		case X86_64_IDIV: fprintf(file, "idiv"); break;

		case X86_64_CDQ: fprintf(file, "cdq"); break;

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
			fprintf(file, "\n");
			return;

		case X86_64_NEG:
		case X86_64_NOT:
		case X86_64_IDIV:
			fprintf(file, " ");
			print_operand(file, &inst->instruction.unary.src);
			fprintf(file, "\n");
			return;

		case X86_64_MOV:
		case X86_64_ADD:
		case X86_64_SUB:
		case X86_64_IMUL:
		case X86_64_AND:
		case X86_64_OR:
		case X86_64_XOR:
		case X86_64_SHL:
		case X86_64_SAR:
			fprintf(file, " ");
			print_operand(file, &inst->instruction.binary.dst);
			fprintf(file, ", ");
			print_operand(file, &inst->instruction.binary.src);
			fprintf(file, "\n");
			return;

		// Pseudo
		case X86_64_ALLOCATE_STACK:
			fputs("push rbp\n", file);
			fputs("mov rbp, rsp\n", file);
			fprintf(file, "sub rsp, %d\n", inst->instruction.unary.src.value.stack);
			return;
		case X86_64_DEALLOCATE_STACK:
			fputs("mov rsp, rbp\n", file);
			fputs("pop rbp\n", file);
			return;

		default: UNIMPLEMENTED("Unhandled mnemonic case");
	}
}
