#include "backend/x86_64/x86_64.h"
#include "backend/x86_64/program.h"
#include "backend/x86_64/regalloc.h"

#include "ir/ir.h"

#include "utils/array.h"
#include "utils/debug.h"

static x86_64Function create_fn(const IRFunction *fn);
static void create_bb(x86_64Function *fn, const IRBasicBlock *bb);
static void create_inst(x86_64Function *fn, const IRInstruction *inst);

void x86_64_create_prog(const IR *ir, x86_64Program *prog)
{
	ASSERT(prog != NULL, "null program");
	ASSERT(ir != NULL, "null IR");

	for (int i = 0; i < array_length(ir->functions); i++) {
		x86_64Function fn = create_fn(&ir->functions[i]);
		array_push(prog->functions, fn);
	}

	x86_64_regalloc(prog);
}

static x86_64Function create_fn(const IRFunction *fn)
{
	// TODO: name
	x86_64Function x86_fn = x86_64_function_init("main");
	for (int i = 0; i < array_length(fn->basic_blocks); i++) {
		create_bb(&x86_fn, &fn->basic_blocks[i]);
	}
	return x86_fn;
}

static void create_bb(x86_64Function *fn, const IRBasicBlock *bb)
{
	for (int i = 0; i < array_length(bb->instructions); i++) {
		create_inst(fn, &bb->instructions[i]);
	}
}

static void create_inst(x86_64Function *fn, const IRInstruction *inst)
{
	switch (inst->type) {
		case IR_RETURN: {
			x86_64Instruction mov = {
				.mnemonic = X86_64_MOV,
				.instruction.mov.dst = {X86_64_REGISTER, .value.reg = X86_64_AX},
				.instruction.mov.src = {X86_64_PSEUDO, .value.pseudo = inst->src1.value},
			};
			x86_64Instruction ret = {
				.mnemonic = X86_64_RET
			};
			array_push(fn->instructions, mov);
			array_push(fn->instructions, ret);
			return;
		}
		case IR_BITWISE_NOT: {
			ASSERT(inst->src1.type == IR_OPERAND_TEMP, "invalid bitwise not: source is not a temporary");
			x86_64Instruction mov = {
				.mnemonic = X86_64_MOV,
				.instruction.mov.src = {X86_64_PSEUDO, .value.pseudo = inst->src1.value},
				.instruction.mov.dst = {X86_64_PSEUDO, .value.pseudo = inst->dest_id},
			};
			x86_64Instruction unary = {
				.mnemonic = X86_64_NOT,
				.instruction.unary.src = {X86_64_PSEUDO, .value.pseudo = inst->dest_id},
			};
			array_push(fn->instructions, mov);
			array_push(fn->instructions, unary);
			return;
		}
		case IR_NEGATE: {
			ASSERT(inst->src1.type == IR_OPERAND_TEMP, "invalid negate: source is not a temporary");
			x86_64Instruction mov = {
				.mnemonic = X86_64_MOV,
				.instruction.mov.src = {X86_64_PSEUDO, .value.pseudo = inst->src1.value},
				.instruction.mov.dst = {X86_64_PSEUDO, .value.pseudo = inst->dest_id},
			};
			x86_64Instruction unary = {
				.mnemonic = X86_64_NEG,
				.instruction.unary.src = {X86_64_PSEUDO, .value.pseudo = inst->dest_id},
			};
			array_push(fn->instructions, mov);
			array_push(fn->instructions, unary);
			return;
		}

		default: UNIMPLEMENTED("Unhandled instruction type case");
	}
}
