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
			IROperand tmp = generate_expression(bb, stmt->node_value.return_statement.expression);
			IRInstruction inst = IR_INSTRUCTION_RETURN(tmp.value);
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
				case AST_UNARY_NEGATE:{
					IRInstruction inst = ir_instruction_unary(IR_NEGATE, &inner_op);
					array_push(bb->instructions, inst);
					return IR_OPERAND_CREATE(IR_OPERAND_TEMP, inst.dest_id);
				}

				default: UNIMPLEMENTED("Unhandled unary case");
			}
			break;
		}
		case AST_BINARY:{
			IROperand left = generate_expression(bb, expr->node_value.binary.left);
			IROperand right = generate_expression(bb, expr->node_value.binary.right);

			IRInstruction inst;
			switch (expr->node_value.binary.type) {
				case AST_BINARY_ADD:
					inst = ir_instruction_binary(IR_ADD, &left, &right); break;
				case AST_BINARY_SUBTRACT:
					inst = ir_instruction_binary(IR_SUBTRACT, &left, &right); break;
				case AST_BINARY_MULTIPLY:
					inst = ir_instruction_binary(IR_MULTIPLY, &left, &right); break;
				case AST_BINARY_DIVIDE:
					inst = ir_instruction_binary(IR_DIVIDE, &left, &right); break;
				case AST_BINARY_REMAINDER:
					inst = ir_instruction_binary(IR_REMAINDER, &left, &right); break;

				default: UNIMPLEMENTED("Unhandled binary case");
			}

			array_push(bb->instructions, inst);
			return IR_OPERAND_CREATE(IR_OPERAND_TEMP, inst.dest_id);
		}

		default: UNIMPLEMENTED("Unhandled expression case");
	}
}
