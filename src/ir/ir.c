#include "ir/ir.h"
#include "ir/basic_block.h"
#include "ir/function.h"
#include "ir/instruction.h"

#include "parser/ast.h"

#include "utils/array.h"
#include "utils/debug.h"

static void generate_function(IR *ir, ASTNode *fn); // generate ir for function
static void generate_statement(IRBasicBlock *bb, ASTNode *stmt); // generate ir for statement
static IRTemporaryID generate_expression(IRBasicBlock *bb, ASTNode *expr); // generate ir for expression

void ir_print(IR *ir)
{
	for (int i = 0; i < array_length(ir->functions); i++) {
		function_print(&ir->functions[i]);
	}
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
			IRTemporaryID tmp = generate_expression(bb, stmt->node_value.return_statement.expression);
			IRInstruction inst = instruction_return(tmp);
			array_push(bb->instructions, inst);
			return;
		}
		default: UNIMPLEMENTED("Unhandled statement type case");
	}

}

static IRTemporaryID generate_expression(IRBasicBlock *bb, ASTNode *expr)
{
	switch (expr->type) {
		case AST_INT_LITERAL:{
			int val = expr->node_value.int_literal.value;
			IRInstruction inst = instruction_store_const(val);
			array_push(bb->instructions, inst);
			return inst.dest_id;
		}
		case AST_UNARY:{
			IRTemporaryID temp = generate_expression(bb, expr->node_value.unary.expression);
			switch (expr->node_value.unary.type) {
				case AST_UNARY_NEGATE:{
					IRInstruction inst = instruction_negate_temp(temp);
					array_push(bb->instructions, inst);
					return inst.dest_id;
				}

				default: UNIMPLEMENTED("Unhandled unary case");
			}
			break;
		}
		case AST_BINARY:{
			IRTemporaryID left = generate_expression(bb, expr->node_value.binary.left);
			IRTemporaryID right = generate_expression(bb, expr->node_value.binary.right);

			IRInstruction inst;
			switch (expr->node_value.binary.type) {
				case AST_BINARY_ADD:
					inst = instruction_add(left, right); break;
				case AST_BINARY_SUBTRACT:
					inst = instruction_sub(left, right); break;
				case AST_BINARY_MULTIPLY:
					inst = instruction_mul(left, right); break;
				case AST_BINARY_DIVIDE:
					inst = instruction_div(left, right); break;
				case AST_BINARY_REMAINDER:
					inst = instruction_rem(left, right); break;

				default: UNIMPLEMENTED("Unhandled binary case");
			}

			array_push(bb->instructions, inst);
			return inst.dest_id;
		}

		default: UNIMPLEMENTED("Unhandled expression case");
	}
}
