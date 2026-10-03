#include <stdbool.h>

#include "ir/ir.h"
#include "ir/instruction.h"

#include "parser/ast.h"

#include "utils/array.h"
#include "utils/debug.h"
#include "utils/stringmap.h"

typedef struct IR {
	IRInstruction *instructions;

	StringMap variable_to_temp;
}IR;
IR *ir = NULL;

static void generate_function(const ASTNode *fn); // generate ir for function
static void generate_statement(const ASTNode *stmt); // generate ir for statement
static IROperand generate_expression(const ASTNode *expr); // generate ir for expression

static IROperand handle_binary_simple(const ASTNode *expr);
static IROperand handle_binary_relational(const ASTNode *expr);
static IROperand handle_binary_short_circuit(const ASTNode *expr);

// Hacky
StringMap *var_to_temp;

void ir_print(void)
{
	for (int i = 0; i < array_length(ir->instructions); i++) {
		instruction_print(&ir->instructions[i]);
	}
	printf("\n");
}

void ir_init(void)
{
	ASSERT(ir == NULL, "ir was already initialized");
	ir = malloc(sizeof(IR));

	ir->instructions = array_create(ir->instructions, 1);
	ir->variable_to_temp = string_map_init();
	var_to_temp = &ir->variable_to_temp;
}

void ir_deinit(void)
{
	if (ir != NULL) {
		if (ir->instructions == NULL) return;
		for (int i = 0; i < array_length(ir->instructions); i++) {
			instruction_free(&ir->instructions[i]);
		}
		array_free(ir->instructions);

		StringMapIterator it = string_map_iterator(&ir->variable_to_temp);
		while(string_map_next(&it)) {
			free(it.entry->value); // all allocated ints for ids
		}
		string_map_deinit(&ir->variable_to_temp);
		free(ir);
	}
}

void ir_generate(const ASTNode *ast)
{
	ASSERT(ast->type == AST_PROGRAM, "invalid ast, type is not AST_PROGRAM");
	generate_function(ast->node_value.program.function);
}

IRInstruction *ir_get_instructions(void)
{
	return ir->instructions;
}

static void generate_function(const ASTNode *fn)
{
	bool had_return = false;
	for (int i = 0; i < array_length(fn->node_value.function.body); i++) {
		ASTNode *node = fn->node_value.function.body[i];
		if (node->type == AST_STATEMENT) {
			if (node->node_value.statement.type == AST_STATEMENT_RETURN)
				had_return = true;
			generate_statement(node);
		} else {
			generate_expression(node);
		}
	}
	if (!had_return) {
		// create return if there isn't one
		IROperand temp = IR_OPERAND_CREATE_TEMP();
		IRInstruction copy = IR_INSTRUCTION_COPY_CONST(temp.value, 0);
		IRInstruction ret = IR_INSTRUCTION_RETURN(temp);
		array_push(ir->instructions, copy);
		array_push(ir->instructions, ret);
	}
}

static void generate_statement(const ASTNode *stmt)
{
	if (stmt->type == AST_STATEMENT) {
		switch (stmt->node_value.statement.type) {
			case AST_STATEMENT_RETURN:{
				IROperand tmp = generate_expression(stmt->node_value.statement.expression);
				IRInstruction inst = IR_INSTRUCTION_RETURN(tmp);

				array_push(ir->instructions, inst);
				return;
			}
			case AST_STATEMENT_NULL_EXPRESSION: return;
			default: UNIMPLEMENTED("Unhandled statement type case");
		}
	}
}

static IROperand generate_expression(const ASTNode *expr)
{
	switch (expr->type) {
		case AST_INT_LITERAL:{
			int val = expr->node_value.int_literal.value;
			return IR_OPERAND_CREATE(IR_OPERAND_CONST, val);
		}
		case AST_UNARY:{
			IROperand inner_op = generate_expression(expr->node_value.unary.expression);
			switch (expr->node_value.unary.type) {
				case AST_UNARY_BITWISE_NOT:{
					IRInstruction inst = ir_instruction_unary(IR_BITWISE_NOT, &inner_op);
					array_push(ir->instructions, inst);
					return IR_OPERAND_CREATE(IR_OPERAND_TEMP, inst.dest_id);
				}
				case AST_UNARY_NEGATE:{
					IRInstruction inst = ir_instruction_unary(IR_NEGATE, &inner_op);
					array_push(ir->instructions, inst);
					return IR_OPERAND_CREATE(IR_OPERAND_TEMP, inst.dest_id);
				}
				case AST_UNARY_LOGICAL_NOT:{
					IRInstruction inst = ir_instruction_unary(IR_LOGICAL_NOT, &inner_op);
					array_push(ir->instructions, inst);
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
					return handle_binary_simple(expr);

				case AST_BINARY_LOGICAL_EQUAL:
				case AST_BINARY_LOGICAL_NOT_EQUAL:
				case AST_BINARY_LOGICAL_LESS_THAN:
				case AST_BINARY_LOGICAL_GREATER_THAN:
				case AST_BINARY_LOGICAL_LESS_EQUAL:
				case AST_BINARY_LOGICAL_GREATER_EQUAL:
					return handle_binary_relational(expr);

				case AST_BINARY_LOGICAL_AND:
				case AST_BINARY_LOGICAL_OR:
					return handle_binary_short_circuit(expr);

				default: UNIMPLEMENTED("Unhandled binary case");
			}
		}
		case AST_VARIABLE:{
			return ir_temp_from_variable(expr->node_value.variable.identifier, var_to_temp);
		}
		case AST_DECLARATION:{
			if (expr->node_value.declaration.init != NULL) {
				IROperand lhs = ir_temp_from_variable(expr->node_value.declaration.name, var_to_temp);
				IROperand rhs = generate_expression(expr->node_value.declaration.init);
				IRInstruction copy = ir_instruction_copy(&lhs, &rhs);
				array_push(ir->instructions, copy);
				return IR_OPERAND_CREATE(IR_OPERAND_TEMP, copy.dest_id);
			}
			// a bit hacky, but this won't be used
			return IR_OPERAND_CREATE(IR_OPERAND_TEMP, -999);
		}
		case AST_ASSIGNMENT:{
			ASSERT(expr->node_value.assignment.left->type == AST_VARIABLE, "invalid lhs");
			IROperand lhs = generate_expression(expr->node_value.assignment.left);
			IROperand rhs = generate_expression(expr->node_value.assignment.right);
			IRInstruction copy = ir_instruction_copy(&lhs, &rhs);
			array_push(ir->instructions, copy);
			return lhs;
		}

		default: UNIMPLEMENTED("Unhandled expression case");
	}
}

static IROperand handle_binary_simple(const ASTNode *expr)
{
	IROperand left = generate_expression(expr->node_value.binary.left);
	IROperand right = generate_expression(expr->node_value.binary.right);

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
	array_push(ir->instructions, inst);
	return IR_OPERAND_CREATE(IR_OPERAND_TEMP, inst.dest_id);
}

static IROperand handle_binary_relational(const ASTNode *expr)
{
	IROperand left = generate_expression(expr->node_value.binary.left);
	IROperand right = generate_expression(expr->node_value.binary.right);

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
	array_push(ir->instructions, inst);
	return IR_OPERAND_CREATE(IR_OPERAND_TEMP, inst.dest_id);
}

static IROperand handle_binary_short_circuit(const ASTNode *expr)
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

	IROperand left = generate_expression(expr->node_value.binary.left);
	IRInstruction jump_cond1 = ir_instruction_branch_condition(jump_type, &left, cond_label_name);
	array_push(ir->instructions, jump_cond1);

	IROperand right = generate_expression(expr->node_value.binary.right);
	IRInstruction jump_cond2 = ir_instruction_branch_condition(jump_type, &right, cond_label_name);
	array_push(ir->instructions, jump_cond2);

	IRInstruction copy_1 = IR_INSTRUCTION_COPY_CONST(temp.value, copy1_val);
	array_push(ir->instructions, copy_1);

	IRInstruction jump_end = ir_instruction_branch_always(IR_JUMP, end_label_name);
	array_push(ir->instructions, jump_end);

	IRInstruction cond_label = ir_instruction_label(cond_label_name);
	array_push(ir->instructions, cond_label);

	IRInstruction copy_2 = IR_INSTRUCTION_COPY_CONST(temp.value, copy2_val);
	array_push(ir->instructions, copy_2);

	IRInstruction end_label = ir_instruction_label(end_label_name);
	array_push(ir->instructions, end_label);

	free(end_label_name);
	free(cond_label_name);
	return temp;
}
