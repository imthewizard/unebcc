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
			x86_64Instruction mov = X64_INSTRUCTION_BINARY(X86_64_MOV,
				X64_OPERAND_REG(X86_64_AX),
				x86_64_ir_operand(&inst->src1)
			);
			x86_64Instruction ret = X64_INSTRUCTION_NO_OPS(X86_64_RET);
			array_push(fn->instructions, mov);
			array_push(fn->instructions, ret);
			return;
		}
		case IR_BITWISE_NOT: {
			x86_64Instruction mov = X64_INSTRUCTION_BINARY(X86_64_MOV,
				X64_OPERAND_PSEUDO(inst->dest_id),
				x86_64_ir_operand(&inst->src1)
			);
			x86_64Instruction unary = X64_INSTRUCTION_UNARY(X86_64_NOT,
				X64_OPERAND_PSEUDO(inst->dest_id)
			);
			array_push(fn->instructions, mov);
			array_push(fn->instructions, unary);
			return;
		}
		case IR_NEGATE: {
			x86_64Instruction mov = X64_INSTRUCTION_BINARY(X86_64_MOV,
				X64_OPERAND_PSEUDO(inst->dest_id),
				x86_64_ir_operand(&inst->src1)
			);
			x86_64Instruction unary = X64_INSTRUCTION_UNARY(X86_64_NEG,
				X64_OPERAND_PSEUDO(inst->dest_id)
			);
			array_push(fn->instructions, mov);
			array_push(fn->instructions, unary);
			return;
		}

		default: UNIMPLEMENTED("Unhandled instruction type case");
	}
}
