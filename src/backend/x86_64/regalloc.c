#include <assert.h>

#include "backend/x86_64/program.h"
#include "backend/x86_64/regalloc.h"
#include "utils/array.h"

static void regalloc(x86_64Instruction *inst, int *offset);

void x86_64_regalloc(x86_64Program *prog)
{
	for (int i = 0; i < array_length(prog->functions); i++) {
		x86_64Function *fn = &prog->functions[i];
		int next_stack_offset = -4;

		for (int j = 0; j < array_length(fn->instructions); j++) {
			x86_64Instruction *inst = &fn->instructions[j];

			regalloc(inst, &next_stack_offset);
		}
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

		default: assert(0);
	}
}
