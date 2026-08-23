#include <assert.h>

#include "ir/ir.h"
#include "ir/basic_block.h"
#include "ir/function.h"
#include "ir/instruction.h"
#include "parser/ast.h"
#include "utils/array.h"

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
	array_free(ir->functions);
}

void ir_generate(IR *ir, ASTNode *ast)
{
	assert(ast->type == AST_PROGRAM);
	generate_function(ir, ast->node_value.program.function);
}

static void generate_function(IR *ir, ASTNode *fn)
{
	assert(fn->type == AST_FUNCTION);

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
		default:
			assert(0); // Unhandled type
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

				default:
					assert(0); // Unhandled type
			}
			assert(0);
			break;
		}

		default:
			assert(0); // Unhandled type
	}
}
