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
/// Specific generation for jumps
static void generic_jump(x86_64Function *fn, const IRInstruction *inst);
/// Specific generation for conditionals
static void generic_conditional(x86_64Function *fn, const IRInstruction *inst);
/// Specific generation for ir_copy
static void generic_copy(x86_64Function *fn, const IRInstruction *inst);
/// Specific generation for ir_label
static void handle_label(x86_64Function *fn, const IRInstruction *inst);

void x86_64_create_prog(const IRFunction *functions, x86_64Program *prog)
{
	ASSERT(prog != NULL, "null program");

	for (int i = 0; i < array_length(functions); i++) {
		x86_64Function fn = create_fn(&functions[i]);
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

		case IR_LOGICAL_NOT:
		case IR_LOGICAL_EQUAL:
		case IR_LOGICAL_LESS_THAN:
		case IR_LOGICAL_LESS_EQUAL:
		case IR_LOGICAL_GREATER_THAN:
		case IR_LOGICAL_GREATER_EQUAL:
		case IR_LOGICAL_NOT_EQUAL:
			generic_conditional(fn, inst); break;

		case IR_COPY:
			generic_copy(fn, inst); break;

		case IR_DIVIDE:
		case IR_REMAINDER:
			generic_binary_div_rem(fn, inst); break;

		case IR_JUMP:
		case IR_JUMP_ZERO:
		case IR_JUMP_NOT_ZERO:
			generic_jump(fn, inst); break;

		case IR_LABEL:
			handle_label(fn, inst); break;

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
		case IR_RIGHT_SHIFT: mnemonic = X86_64_SAR; break;

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

static void generic_jump(x86_64Function *fn, const IRInstruction *inst)
{
	if (inst->type == IR_JUMP_ZERO) {
		char *label_target = inst->src2.label;
		x86_64Instruction cmp = X64_INSTRUCTION_BINARY(X86_64_CMP,
			x86_64_ir_operand(&inst->src1),
			X64_OPERAND_IMM(0)
		);
		x86_64Instruction je = X64_INSTRUCTION_JUMP(X86_64_JE,
			label_target
		);
		array_push(fn->instructions, cmp);
		array_push(fn->instructions, je);
		return;
	}
	if (inst->type == IR_JUMP_NOT_ZERO) {
		char *label_target = inst->src2.label;
		x86_64Instruction cmp = X64_INSTRUCTION_BINARY(X86_64_CMP,
			x86_64_ir_operand(&inst->src1),
			X64_OPERAND_IMM(0)
		);
		x86_64Instruction jne = X64_INSTRUCTION_JUMP(X86_64_JNE,
			label_target
		);
		array_push(fn->instructions, cmp);
		array_push(fn->instructions, jne);
		return;
	}
	if (inst->type == IR_JUMP) {
		char *label_target = inst->src1.label;
		x86_64Instruction jmp = X64_INSTRUCTION_JUMP(X86_64_JMP,
			label_target
		);
		array_push(fn->instructions, jmp);
		return;
	}

	ASSERT(0, "unhandled jump case");
}

static void generic_conditional(x86_64Function *fn, const IRInstruction *inst)
{
	x86_64Instruction cmp;
	if (inst->type != IR_LOGICAL_NOT) {
		cmp = X64_INSTRUCTION_BINARY(X86_64_CMP,
			x86_64_ir_operand(&inst->src1),
			x86_64_ir_operand(&inst->src2)
		);
	} else {
		cmp = X64_INSTRUCTION_BINARY(X86_64_CMP,
			x86_64_ir_operand(&inst->src1),
			X64_OPERAND_IMM(0)
		);
	}
	x86_64Instruction mov = X64_INSTRUCTION_BINARY(X86_64_MOV,
		X64_OPERAND_PSEUDO(inst->dest_id),
		X64_OPERAND_IMM(0)
	);

	x86_64Mnemonics set_mnemonic;
	switch (inst->type) {
		case IR_LOGICAL_NOT: set_mnemonic = X86_64_SETE; break;
		case IR_LOGICAL_EQUAL: set_mnemonic = X86_64_SETE; break;
		case IR_LOGICAL_LESS_THAN: set_mnemonic = X86_64_SETL; break;
		case IR_LOGICAL_LESS_EQUAL: set_mnemonic = X86_64_SETLE; break;
		case IR_LOGICAL_GREATER_THAN: set_mnemonic = X86_64_SETG; break;
		case IR_LOGICAL_GREATER_EQUAL: set_mnemonic = X86_64_SETGE; break;
		case IR_LOGICAL_NOT_EQUAL: set_mnemonic = X86_64_SETNE; break;

		default: UNIMPLEMENTED("unhandled instruction type");
	}

	x86_64Instruction setcc = X64_INSTRUCTION_UNARY(set_mnemonic,
		X64_OPERAND_PSEUDO(inst->dest_id)
	);

	array_push(fn->instructions, cmp);
	array_push(fn->instructions, mov);
	array_push(fn->instructions, setcc);
}

static void generic_copy(x86_64Function *fn, const IRInstruction *inst)
{
	ASSERT(inst->type == IR_COPY, "type is not IR_COPY");
	x86_64Instruction mov = X64_INSTRUCTION_BINARY(X86_64_MOV,
		X64_OPERAND_PSEUDO(inst->src1.value),
		x86_64_ir_operand(&inst->src2)
	);
	array_push(fn->instructions, mov);
}

static void handle_label(x86_64Function *fn, const IRInstruction *inst)
{
	ASSERT(inst->type == IR_LABEL, "type is not IR_LABEL");
	x86_64Instruction label = X64_INSTRUCTION_DEFINE_LABEL(inst->src1.label);
	array_push(fn->instructions, label);
}
