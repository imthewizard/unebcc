#include "backend/x86_64/x86_64.h"
#include "backend/x86_64/program.h"
#include "backend/x86_64/regalloc.h"

#include "ir/instruction.h"
#include "ir/ir.h"

#include "utils/array.h"
#include "utils/debug.h"

static x86_64Function create_fn(const IRFunction *fn);
static void create_bb(x86_64Function *fn, const IRBasicBlock *bb);
static void create_inst(x86_64Function *fn, const IRInstruction *inst);

/// Generic unary instructions
static void generic_unary(x86_64Function *fn, const IRInstruction *inst);
/// Generic binary instructions
static void generic_binary(x86_64Function *fn, const IRInstruction *inst);
/// Specific generation for division and remainder
static void generic_binary_div_rem(x86_64Function *fn, const IRInstruction *inst);

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
		case IR_BITWISE_NOT:
		case IR_NEGATE:
			generic_unary(fn, inst); break;

		case IR_ADD:
		case IR_SUBTRACT:
		case IR_MULTIPLY:
		case IR_BITWISE_AND:
		case IR_BITWISE_OR:
		case IR_BITWISE_XOR:
		case IR_LEFT_SHIFT:
		case IR_RIGHT_SHIFT:
			generic_binary(fn, inst); break;

		case IR_DIVIDE:
		case IR_REMAINDER:
			generic_binary_div_rem(fn, inst); break;

		default: UNIMPLEMENTED("Unhandled instruction type case");
	}
}

static void generic_unary(x86_64Function *fn, const IRInstruction *inst)
{
	x86_64Mnemonics mnemonic;
	switch (inst->type) {
		case IR_BITWISE_NOT: mnemonic = X86_64_NOT; break;
		case IR_NEGATE: mnemonic = X86_64_NEG; break;

		default: UNIMPLEMENTED("unhandled inst type");
	}

	x86_64Instruction mov = X64_INSTRUCTION_BINARY(X86_64_MOV,
		X64_OPERAND_PSEUDO(inst->dest_id),
		x86_64_ir_operand(&inst->src1)
	);
	x86_64Instruction unary = X64_INSTRUCTION_UNARY(mnemonic,
		X64_OPERAND_PSEUDO(inst->dest_id)
	);
	array_push(fn->instructions, mov);
	array_push(fn->instructions, unary);
	return;
}

static void generic_binary(x86_64Function *fn, const IRInstruction *inst)
{
	x86_64Mnemonics mnemonic;
	switch (inst->type) {
		case IR_ADD: mnemonic = X86_64_ADD; break;
		case IR_SUBTRACT: mnemonic = X86_64_SUB; break;
		case IR_MULTIPLY: mnemonic = X86_64_IMUL; break;
		case IR_BITWISE_AND: mnemonic = X86_64_AND; break;
		case IR_BITWISE_OR: mnemonic = X86_64_OR; break;
		case IR_BITWISE_XOR: mnemonic = X86_64_XOR; break;
		case IR_LEFT_SHIFT: mnemonic = X86_64_SHL; break;
		case IR_RIGHT_SHIFT: mnemonic = X86_64_SHR; break;

		default: UNIMPLEMENTED("unhandled inst type");
	}

	x86_64Instruction mov = X64_INSTRUCTION_BINARY(X86_64_MOV,
		X64_OPERAND_PSEUDO(inst->dest_id),
		x86_64_ir_operand(&inst->src1)
	);
	x86_64Instruction binary = X64_INSTRUCTION_BINARY(mnemonic,
		X64_OPERAND_PSEUDO(inst->dest_id),
		x86_64_ir_operand(&inst->src2)
	);
	array_push(fn->instructions, mov);
	array_push(fn->instructions, binary);
	return;
}

static void generic_binary_div_rem(x86_64Function *fn, const IRInstruction *inst)
{
	x86_64Operand reg_op;
	switch (inst->type) {
		case IR_DIVIDE: reg_op = X64_OPERAND_REG(X86_64_AX); break;
		case IR_REMAINDER: reg_op = X64_OPERAND_REG(X86_64_DX); break;
		default: UNIMPLEMENTED("unhandled inst type");
	}
	x86_64Instruction mov1 = X64_INSTRUCTION_BINARY(X86_64_MOV,
		X64_OPERAND_REG(X86_64_AX),
		x86_64_ir_operand(&inst->src1)
	);
	x86_64Instruction cdq = X64_INSTRUCTION_NO_OPS(X86_64_CDQ);
	x86_64Instruction idiv = X64_INSTRUCTION_UNARY(X86_64_IDIV,
		x86_64_ir_operand(&inst->src2)
	);
	x86_64Instruction mov2 = X64_INSTRUCTION_BINARY(X86_64_MOV,
		X64_OPERAND_PSEUDO(inst->dest_id),
		reg_op
	);
	array_push(fn->instructions, mov1);
	array_push(fn->instructions, cdq);
	array_push(fn->instructions, idiv);
	array_push(fn->instructions, mov2);
	return;

}
