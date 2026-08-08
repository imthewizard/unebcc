#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ir/ir.h"
#include "ir/basic_block.h"
#include "ir/function.h"
#include "ir/instruction.h"
#include "parser/ast.h"

// Pushes a function to the IR
static void ir_push_func(IR *ir, IRFunction *fn);

static void generate_function(IR *ir, ASTNode *fn); // generate ir for function
static void generate_statement(IRBasicBlock *bb, ASTNode *stmt); // generate ir for statement
static IRTemporaryID generate_expression(IRBasicBlock *bb, ASTNode *expr); // generate ir for expression

static void ir_push_func(IR *ir, IRFunction *fn)
{
	IRFunction *tmp = realloc(ir->functions, sizeof(IRFunction) * (ir->function_amount + 1));
	if (tmp == NULL) {
		puts("ir_push_func: realloc fail");
		exit(EXIT_FAILURE);
	}

	ir->functions = tmp;
	ir->functions[ir->function_amount++] = *fn;
}

void ir_print(IR *ir)
{
	for (int i = 0; i < ir->function_amount; i++) {
		function_print(&ir->functions[i]);
	}
}

void ir_init(IR *ir)
{
	ir->functions = NULL;
	ir->function_amount = 0;
}

void ir_deinit(IR *ir)
{
	if (ir->functions == NULL) return;

	for (int i = 0; i < ir->function_amount; i++) {
		function_deinit(&ir->functions[i]);
	}

	free(ir->functions);
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

	function_push(&ir_fn, &fn_bb);
	ir_push_func(ir, &ir_fn);
}

static void generate_statement(IRBasicBlock *bb, ASTNode *stmt)
{
	switch (stmt->type) {
		case AST_RETURN_STATEMENT:{
			IRTemporaryID tmp = generate_expression(bb, stmt->node_value.return_statement.expression);
			IRInstruction inst = instruction_return(tmp);
			basic_block_push(bb, &inst);
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
			basic_block_push(bb, &inst);
			return inst.dest_id;
		}
		case AST_UNARY:{
			IRTemporaryID temp = generate_expression(bb, expr->node_value.unary.expression);
			switch (expr->node_value.unary.type) {
				case AST_UNARY_NEGATE:{
					IRInstruction inst = instruction_negate_temp(temp);
					basic_block_push(bb, &inst);
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
