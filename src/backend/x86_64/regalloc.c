#include "backend/x86_64/program.h"
#include "backend/x86_64/regalloc.h"
#include "backend/x86_64/x86_64.h"

#include "utils/array.h"
#include "utils/debug.h"

static void regalloc(x86_64Instruction *inst, int *offset);
static void stackalloc(x86_64Function *fn, int alloc_amount);
static void fix_invalid_movs(x86_64Function *fn);

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
		fix_invalid_movs(fn);
	}
}

static void regalloc(x86_64Instruction *inst, int *next_offset)
{
	// TODO: should probably be a hashmap, but works for now
	static int pseudo_offset_map[1024] = {0};

	switch (inst->mnemonic) {
		case X86_64_MOV:
			if (inst->instruction.mov.dst.type == X86_64_PSEUDO) {
				int pseudo_id = inst->instruction.mov.dst.value.pseudo;
				int map_off = pseudo_offset_map[pseudo_id];

				if (map_off == 0) {
					// Not mapped yet
					pseudo_offset_map[pseudo_id] = *next_offset;
					*next_offset -= 4;
				}

				inst->instruction.mov.dst.type = X86_64_STACK;
				inst->instruction.mov.dst.value.stack = pseudo_offset_map[pseudo_id];
			}
			if (inst->instruction.mov.src.type == X86_64_PSEUDO) {
				int pseudo_id = inst->instruction.mov.src.value.pseudo;
				int map_off = pseudo_offset_map[pseudo_id];

				if (map_off == 0) {
					// Not mapped yet
					pseudo_offset_map[pseudo_id] = *next_offset;
					*next_offset -= 4;
				}

				inst->instruction.mov.src.type = X86_64_STACK;
				inst->instruction.mov.src.value.stack = pseudo_offset_map[pseudo_id];
			}
			return;

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

	x86_64Instruction sa = {
		.mnemonic = X86_64_ALLOCATE_STACK,
		.instruction.stack.value = alloc_amount,
	};
	x86_64Instruction sd = {
		.mnemonic = X86_64_DEALLOCATE_STACK,
	};

	array_insert(fn->instructions, sa, 0);
	array_insert(fn->instructions, sd, array_length(fn->instructions) - 1);
}

static void fix_invalid_movs(x86_64Function *fn)
{
	for (int i = 0; i < array_length(fn->instructions); i++) {
		x86_64Instruction *inst = &fn->instructions[i];

		if (inst->mnemonic == X86_64_MOV) {
			x86_64Operand *dst = &inst->instruction.mov.dst;
			x86_64Operand *src = &inst->instruction.mov.src;

			// is this a mov [STACK2], [STACK1]?
			if (dst->type == X86_64_STACK && src->type == X86_64_STACK) {
				int old_dst_stack = dst->value.stack;

				// Change this instruction to mov r10, [STACK1]
				dst->type = X86_64_REGISTER;
				dst->value.reg = X86_64_R10;

				// Place a mov [STACK2], r10
				x86_64Instruction new = {
					.mnemonic = X86_64_MOV,
					.instruction.mov.dst = {X86_64_STACK, .value.stack = old_dst_stack},
					.instruction.mov.src = {X86_64_REGISTER, .value.reg = X86_64_R10},
				};
				array_insert(fn->instructions, new, i + 1);
			}
		}
	}
}
