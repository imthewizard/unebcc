#include <stdbool.h>

#include "ir/ir.h"
#include "ir/basic_block.h"
#include "ir/function.h"
#include "ir/instruction.h"

#include "parser/ast.h"

#include "utils/array.h"
#include "utils/debug.h"

static void generate_function(IR *ir, ASTNode *fn); // generate ir for function
static void generate_statement(IRBasicBlock *bb, ASTNode *stmt); // generate ir for statement
static IROperand generate_expression(IRBasicBlock *bb, ASTNode *expr); // generate ir for expression

static IROperand handle_binary_simple(IRBasicBlock *bb, ASTNode *expr);
static IROperand handle_binary_relational(IRBasicBlock *bb, ASTNode *expr);
static IROperand handle_binary_short_circuit(IRBasicBlock *bb, ASTNode *expr);

void ir_print(IR *ir)
{
	for (int i = 0; i < array_length(ir->functions); i++) {
		function_print(&ir->functions[i]);
	}
	printf("\n");
}

void ir_init(IR *ir)
{
	ir->functions = array_create(ir->functions, 1);
}

void ir_deinit(IR *ir)
{
	if (ir->functions == NULL) return;
	for (int i = 0; i < array_length(ir->functions); i++) {
		function_deinit(&ir->functions[i]);
	}
	array_free(ir->functions);
}

void ir_generate(IR *ir, ASTNode *ast)
{
	ASSERT(ast->type == AST_PROGRAM, "invalid ast, type is not AST_PROGRAM");
	generate_function(ir, ast->node_value.program.function);
}

static void generate_function(IR *ir, ASTNode *fn)
{
	IRFunction ir_fn;
	function_init(&ir_fn);

	IRBasicBlock fn_bb;
	basic_block_init(&fn_bb);

	generate_statement(&fn_bb, fn->node_value.function.body);

	array_push(ir_fn.basic_blocks, fn_bb);
	array_push(ir->functions, ir_fn);
}

static void generate_statement(IRBasicBlock *bb, ASTNode *stmt)
{
	switch (stmt->type) {
		case AST_RETURN_STATEMENT:{
			IROperand tmp = generate_expression(bb, stmt->node_value.return_statement.expression);
			IRInstruction inst = IR_INSTRUCTION_RETURN(tmp);

			array_push(bb->instructions, inst);
			return;
		}
		default: UNIMPLEMENTED("Unhandled statement type case");
	}

}

static IROperand generate_expression(IRBasicBlock *bb, ASTNode *expr)
{
	switch (expr->type) {
		case AST_INT_LITERAL:{
			int val = expr->node_value.int_literal.value;
			return IR_OPERAND_CREATE(IR_OPERAND_CONST, val);
		}
		case AST_UNARY:{
			IROperand inner_op = generate_expression(bb, expr->node_value.unary.expression);
			switch (expr->node_value.unary.type) {
				case AST_UNARY_BITWISE_NOT:{
					IRInstruction inst = ir_instruction_unary(IR_BITWISE_NOT, &inner_op);
					array_push(bb->instructions, inst);
					return IR_OPERAND_CREATE(IR_OPERAND_TEMP, inst.dest_id);
				}
				case AST_UNARY_NEGATE:{
					IRInstruction inst = ir_instruction_unary(IR_NEGATE, &inner_op);
					array_push(bb->instructions, inst);
					return IR_OPERAND_CREATE(IR_OPERAND_TEMP, inst.dest_id);
				}
				case AST_UNARY_LOGICAL_NOT:{
					IRInstruction inst = ir_instruction_unary(IR_LOGICAL_NOT, &inner_op);
					array_push(bb->instructions, inst);
					return IR_OPERAND_CREATE(IR_OPERAND_TEMP, inst.dest_id);
				}

				default: UNIMPLEMENTED("Unhandled unary case");
			}
			break;
		}
		case AST_BINARY:{
			switch (expr->node_value.binary.type) {
				case AST_BINARY_ADD:
				case AST_BINARY_SUBTRACT:
				case AST_BINARY_MULTIPLY:
				case AST_BINARY_DIVIDE:
				case AST_BINARY_REMAINDER:
				case AST_BINARY_BITWISE_AND:
				case AST_BINARY_BITWISE_OR:
				case AST_BINARY_BITWISE_XOR:
				case AST_BINARY_LEFT_SHIFT:
				case AST_BINARY_RIGHT_SHIFT:
					return handle_binary_simple(bb, expr);

				case AST_BINARY_LOGICAL_EQUAL:
				case AST_BINARY_LOGICAL_NOT_EQUAL:
				case AST_BINARY_LOGICAL_LESS_THAN:
				case AST_BINARY_LOGICAL_GREATER_THAN:
				case AST_BINARY_LOGICAL_LESS_EQUAL:
				case AST_BINARY_LOGICAL_GREATER_EQUAL:
					return handle_binary_relational(bb, expr);

				case AST_BINARY_LOGICAL_AND:
				case AST_BINARY_LOGICAL_OR:
					return handle_binary_short_circuit(bb, expr);

				default: UNIMPLEMENTED("Unhandled binary case");
			}
		}

		default: UNIMPLEMENTED("Unhandled expression case");
	}
}

static IROperand handle_binary_simple(IRBasicBlock *bb, ASTNode *expr)
{
	IROperand left = generate_expression(bb, expr->node_value.binary.left);
	IROperand right = generate_expression(bb, expr->node_value.binary.right);

	IRInstructionType type;

	switch(expr->node_value.binary.type) {
		case AST_BINARY_ADD: type = IR_ADD; break;
		case AST_BINARY_SUBTRACT: type = IR_SUBTRACT; break;
		case AST_BINARY_MULTIPLY: type = IR_MULTIPLY; break;
		case AST_BINARY_DIVIDE: type = IR_DIVIDE; break;
		case AST_BINARY_REMAINDER: type = IR_REMAINDER; break;
		case AST_BINARY_BITWISE_AND: type = IR_BITWISE_AND; break;
		case AST_BINARY_BITWISE_OR: type = IR_BITWISE_OR; break;
		case AST_BINARY_BITWISE_XOR: type = IR_BITWISE_XOR; break;
		case AST_BINARY_LEFT_SHIFT: type = IR_LEFT_SHIFT; break;
		case AST_BINARY_RIGHT_SHIFT: type = IR_RIGHT_SHIFT; break;

		default: UNIMPLEMENTED("unhandled ast node type");
	}

	IRInstruction inst = ir_instruction_binary(type, &left, &right);
	array_push(bb->instructions, inst);
	return IR_OPERAND_CREATE(IR_OPERAND_TEMP, inst.dest_id);
}

static IROperand handle_binary_relational(IRBasicBlock *bb, ASTNode *expr)
{
	IROperand left = generate_expression(bb, expr->node_value.binary.left);
	IROperand right = generate_expression(bb, expr->node_value.binary.right);

	IRInstructionType type;
	switch (expr->node_value.binary.type) {
		case AST_BINARY_LOGICAL_EQUAL: type = IR_LOGICAL_EQUAL; break;
		case AST_BINARY_LOGICAL_NOT_EQUAL: type = IR_LOGICAL_NOT_EQUAL; break;
		case AST_BINARY_LOGICAL_LESS_THAN: type = IR_LOGICAL_LESS_THAN; break;
		case AST_BINARY_LOGICAL_GREATER_THAN: type = IR_LOGICAL_GREATER_THAN; break;
		case AST_BINARY_LOGICAL_LESS_EQUAL: type = IR_LOGICAL_LESS_EQUAL; break;
		case AST_BINARY_LOGICAL_GREATER_EQUAL: type = IR_LOGICAL_GREATER_EQUAL; break;

		default: UNIMPLEMENTED("unhandled logical type");
	}

	IRInstruction inst = ir_instruction_binary(type, &left, &right);
	array_push(bb->instructions, inst);
	return IR_OPERAND_CREATE(IR_OPERAND_TEMP, inst.dest_id);
}

static IROperand handle_binary_short_circuit(IRBasicBlock *bb, ASTNode *expr)
{
	ASSERT(expr->node_value.binary.type == AST_BINARY_LOGICAL_AND ||
			expr->node_value.binary.type == AST_BINARY_LOGICAL_OR,
			"type is not short circuit type");
	bool is_and = expr->node_value.binary.type == AST_BINARY_LOGICAL_AND;

	char *cond_label_name = is_and ?
		instruction_generate_label("false") :
		instruction_generate_label("true");
	char *end_label_name = instruction_generate_label("end");

	IROperand temp = IR_OPERAND_CREATE_TEMP();

	IRInstructionType jump_type = expr->node_value.binary.type == AST_BINARY_LOGICAL_AND ? IR_JUMP_ZERO : IR_JUMP_NOT_ZERO;
	int copy1_val = is_and ? 1 : 0;
	int copy2_val = copy1_val == 1 ? 0 : 1;

	IROperand left = generate_expression(bb, expr->node_value.binary.left);
	IRInstruction jump_cond1 = ir_instruction_branch_condition(jump_type, &left, cond_label_name);
	array_push(bb->instructions, jump_cond1);

	IROperand right = generate_expression(bb, expr->node_value.binary.right);
	IRInstruction jump_cond2 = ir_instruction_branch_condition(jump_type, &right, cond_label_name);
	array_push(bb->instructions, jump_cond2);

	IRInstruction copy_1 = IR_INSTRUCTION_COPY(temp.value, copy1_val);
	array_push(bb->instructions, copy_1);

	IRInstruction jump_end = ir_instruction_branch_always(IR_JUMP, end_label_name);
	array_push(bb->instructions, jump_end);

	IRInstruction cond_label = ir_instruction_label(cond_label_name);
	array_push(bb->instructions, cond_label);

	IRInstruction copy_2 = IR_INSTRUCTION_COPY(temp.value, copy2_val);
	array_push(bb->instructions, copy_2);

	IRInstruction end_label = ir_instruction_label(end_label_name);
	array_push(bb->instructions, end_label);

	free(end_label_name);
	free(cond_label_name);
	return temp;
}
