#include <stdio.h>

#include "backend/x86_64/emitter.h"
#include "backend/x86_64/x86_64.h"
#include "utils/array.h"
#include "utils/debug.h"
#include "utils/os.h"

static void print_setup(FILE *file);
static void print_end(FILE *file);
static void print_operand(FILE *file, const x86_64Operand *op, int register_size);
static void emit_instruction(FILE *file, const x86_64Instruction *inst);

void x86_64_emit(FILE *file, const x86_64Program *prog)
{
	print_setup(file);

	fprintf(file, "main:\n");
	for (int j = 0; j < array_length(prog->instructions); j++) {
		x86_64Instruction *inst = &prog->instructions[j];
		emit_instruction(file, inst);
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

static void print_operand(FILE *file, const x86_64Operand *op, int register_size)
{
	switch (op->type) {
		case X86_64_IMMEDIATE:
			fprintf(file, "%d", op->value.imm);
			break;
		case X86_64_REGISTER:
			fprintf(file, "%s", x86_64_reg_to_str(op->value.reg, register_size));
			break;
		case X86_64_STACK:
			// spacing after operator
			if (op->value.stack < 0) {
				if (register_size == 1) {
					fprintf(file, "byte ptr [rbp - %d]", -op->value.stack);
				} else {
					fprintf(file, "dword ptr [rbp - %d]", -op->value.stack);
				}
			} else {
				if (register_size == 1) {
					fprintf(file, "byte ptr [rbp + %d]", -op->value.stack);
				} else {
					fprintf(file, "dword ptr [rbp + %d]", -op->value.stack);
				}
			}
			break;
		case X86_64_LABEL:
			fprintf(file, ".%s", op->value.label);
			break;

		default: ASSERT(0, "invalid type: missing or failed regalloc");
	}
}

static void emit_instruction(FILE *file, const x86_64Instruction *inst)
{
	// Print instruction
	if (inst->mnemonic != X86_64_ALLOCATE_STACK &&
		inst->mnemonic != X86_64_DEALLOCATE_STACK &&
		inst->mnemonic != X86_64_DEFINE_LABEL)
		fprintf(file, "%s", x86_64_mnemonic_to_str(inst->mnemonic));

	// Print operands
	switch (inst->mnemonic) {
		case X86_64_RET:
		case X86_64_CDQ:
			fprintf(file, "\n");
			return;

		case X86_64_NEG:
		case X86_64_NOT:
		case X86_64_IDIV:
		case X86_64_JMP:
		case X86_64_JE:
		case X86_64_JNE:
		case X86_64_JG:
		case X86_64_JGE:
		case X86_64_JL:
		case X86_64_JLE:
			fprintf(file, " ");
			print_operand(file, &inst->instruction.unary.src, 4);
			fprintf(file, "\n");
			return;

		case X86_64_SETE:
		case X86_64_SETNE:
		case X86_64_SETG:
		case X86_64_SETGE:
		case X86_64_SETL:
		case X86_64_SETLE:
			fprintf(file, " ");
			print_operand(file, &inst->instruction.unary.src, 1);
			fprintf(file, "\n");
			break;

		case X86_64_MOV:
		case X86_64_ADD:
		case X86_64_SUB:
		case X86_64_IMUL:
		case X86_64_AND:
		case X86_64_OR:
		case X86_64_XOR:
		case X86_64_CMP:
			fprintf(file, " ");
			print_operand(file, &inst->instruction.binary.dst, 4);
			fprintf(file, ", ");
			print_operand(file, &inst->instruction.binary.src, 4);
			fprintf(file, "\n");
			return;

		case X86_64_SHL:
		case X86_64_SAR:
			fprintf(file, " ");
			print_operand(file, &inst->instruction.binary.dst, 4);
			fprintf(file, ", ");
			print_operand(file, &inst->instruction.binary.src, 1);
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
		case X86_64_DEFINE_LABEL:
			print_operand(file, &inst->instruction.unary.src, 0);
			fprintf(file, ":\n");
			return;

		default: UNIMPLEMENTED("Unhandled mnemonic case");
	}
}
