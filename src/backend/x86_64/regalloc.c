#include "backend/x86_64/program.h"
#include "backend/x86_64/regalloc.h"
#include "backend/x86_64/x86_64.h"

#include "utils/array.h"
#include "utils/debug.h"

// Maps the IR's pseudo values to registers
static void regalloc(x86_64Instruction *inst, int *offset);
// Handles the function prologue and epilogue
static void stackalloc(x86_64Function *fn, int alloc_amount);

// Some instructions can't have memory addresses as both dst and src
static void fix_invalid_addr_addr(x86_64Function *fn);
// imul cannot have a memory address as its destination
static void fix_invalid_imuls(x86_64Function *fn);
// idiv cannot have immediates as operands
static void fix_invalid_idivs(x86_64Function *fn);

void x86_64_regalloc(x86_64Program *prog)
{
	for (int i = 0; i < array_length(prog->functions); i++) {
		x86_64Function *fn = &prog->functions[i];
		int next_stack_offset = -4;

		for (int j = 0; j < array_length(fn->instructions); j++) {
			x86_64Instruction *inst = &fn->instructions[j];

			regalloc(inst, &next_stack_offset);
		}

		stackalloc(fn, next_stack_offset + 4);
		fix_invalid_addr_addr(fn);
		fix_invalid_imuls(fn);
		fix_invalid_idivs(fn);
	}
}

static void regalloc(x86_64Instruction *inst, int *next_offset)
{
	// TODO: should probably be a hashmap, but works for now
	static int pseudo_offset_map[1024] = {0};

	switch (inst->mnemonic) {
		case X86_64_ADD:
		case X86_64_SUB:
		case X86_64_IMUL:
		case X86_64_MOV:
			if (inst->instruction.binary.dst.type == X86_64_PSEUDO) {
				int pseudo_id = inst->instruction.binary.dst.value.pseudo;
				int map_off = pseudo_offset_map[pseudo_id];

				if (map_off == 0) {
					// Not mapped yet
					pseudo_offset_map[pseudo_id] = *next_offset;
					*next_offset -= 4;
				}

				inst->instruction.binary.dst.type = X86_64_STACK;
				inst->instruction.binary.dst.value.stack = pseudo_offset_map[pseudo_id];
			}
			if (inst->instruction.binary.src.type == X86_64_PSEUDO) {
				int pseudo_id = inst->instruction.binary.src.value.pseudo;
				int map_off = pseudo_offset_map[pseudo_id];

				if (map_off == 0) {
					// Not mapped yet
					pseudo_offset_map[pseudo_id] = *next_offset;
					*next_offset -= 4;
				}

				inst->instruction.binary.src.type = X86_64_STACK;
				inst->instruction.binary.src.value.stack = pseudo_offset_map[pseudo_id];
			}
			return;

		case X86_64_IDIV:
		case X86_64_NEG:
		case X86_64_NOT:
			if (inst->instruction.unary.src.type == X86_64_PSEUDO) {
				int pseudo_id = inst->instruction.unary.src.value.pseudo;
				int map_off = pseudo_offset_map[pseudo_id];

				if (map_off == 0) {
					// Not mapped yet
					pseudo_offset_map[pseudo_id] = *next_offset;
					*next_offset -= 4;
				}

				inst->instruction.unary.src.type = X86_64_STACK;
				inst->instruction.unary.src.value.stack = pseudo_offset_map[pseudo_id];
			}

		case X86_64_CDQ:
		case X86_64_RET:
			return;

		default: UNIMPLEMENTED("Unhandled mnemonic case");
	}
}

static void stackalloc(x86_64Function *fn, int alloc_amount)
{
	if (alloc_amount == 0) return;

	// abs
	ASSERT(alloc_amount < 0, "invalid alloc_amount");
	alloc_amount = -alloc_amount;

	x86_64Instruction sa = X64_INSTRUCTION_STACK(X86_64_ALLOCATE_STACK, alloc_amount);
	x86_64Instruction sd = X64_INSTRUCTION_NO_OPS(X86_64_DEALLOCATE_STACK);

	array_insert(fn->instructions, sa, 0);
	array_insert(fn->instructions, sd, array_length(fn->instructions) - 1);
}

static void fix_invalid_addr_addr(x86_64Function *fn)
{
	for (int i = 0; i < array_length(fn->instructions); i++) {
		x86_64Instruction *inst = &fn->instructions[i];

		if (inst->mnemonic == X86_64_MOV ||
			inst->mnemonic == X86_64_ADD ||
			inst->mnemonic == X86_64_SUB) {
			x86_64Operand *dst = &inst->instruction.binary.dst;
			x86_64Operand *src = &inst->instruction.binary.src;

			// is this a [mnemonic] [STACK2], [STACK1]?
			if (dst->type == X86_64_STACK && src->type == X86_64_STACK) {
				int old_dst_stack = dst->value.stack;
				int old_src_stack = src->value.stack;
				x86_64Mnemonics old_mnemonic = inst->mnemonic;

				// Change this instruction to mov r10, [STACK1]
				inst->mnemonic = X86_64_MOV;
				inst->instruction.binary.dst = X64_OPERAND_REG(X86_64_R10);
				inst->instruction.binary.src = X64_OPERAND_STACK(old_src_stack);

				// Place a [mnemonic] [STACK2], r10
				x86_64Instruction new = X64_INSTRUCTION_BINARY(old_mnemonic,
					X64_OPERAND_STACK(old_dst_stack),
					X64_OPERAND_REG(X86_64_R10)
				);
				array_insert(fn->instructions, new, i + 1);
			}
		}
	}
}

static void fix_invalid_imuls(x86_64Function *fn)
{
	for (int i = 0; i < array_length(fn->instructions); i++) {
		x86_64Instruction *inst = &fn->instructions[i];
		x86_64Operand *dst = &inst->instruction.binary.dst;

		// is this an imul [STACK], [operand]?
		if (inst->mnemonic == X86_64_IMUL && dst->type == X86_64_STACK) {
			x86_64Operand *src = &inst->instruction.binary.src;

			int old_dst_stack = dst->value.stack;
			x86_64Operand old_src = *src;

			// Change this instruction to mov r11d, [STACK1]
			inst->mnemonic = X86_64_MOV;
			inst->instruction.binary.dst = X64_OPERAND_REG(X86_64_R11);
			inst->instruction.binary.src = X64_OPERAND_STACK(old_dst_stack);

			// Place a imul r11d, [operand]
			x86_64Instruction new_imul = X64_INSTRUCTION_BINARY(X86_64_IMUL,
				X64_OPERAND_REG(X86_64_R11),
				old_src
			);
			array_insert(fn->instructions, new_imul, i + 1);

			// Place a mov [STACK], r11d
			x86_64Instruction new_mov = X64_INSTRUCTION_BINARY(X86_64_MOV,
				X64_OPERAND_STACK(old_dst_stack),
				X64_OPERAND_REG(X86_64_R11)
			);
			array_insert(fn->instructions, new_mov, i + 2);
		}
	}
}

static void fix_invalid_idivs(x86_64Function *fn)
{
	for (int i = 0; i < array_length(fn->instructions); i++) {
		x86_64Instruction *inst = &fn->instructions[i];

		if (inst->mnemonic == X86_64_IDIV) {
			x86_64Operand *src = &inst->instruction.unary.src;

			// is this a idiv [IMMEDIATE]?
			if (src->type == X86_64_IMMEDIATE) {
				int old_immediate = src->value.imm;

				// Change this instruction to mov r10d, [IMMEDIATE]
				inst->mnemonic = X86_64_MOV;
				inst->instruction.binary.dst = X64_OPERAND_REG(X86_64_R10);
				inst->instruction.binary.src = X64_OPERAND_IMM(old_immediate);

				// Place a idiv r10d
				x86_64Instruction new = X64_INSTRUCTION_UNARY(X86_64_IDIV,
					X64_OPERAND_REG(X86_64_R10)
				);
				array_insert(fn->instructions, new, i + 1);
			}
		}
	}
}
