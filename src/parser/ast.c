#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/ast.h"
#include "utils/array.h"

#define INDENT_PER_LEVEL 2

#define PRINT_INDENT(indent, str)\
	do { \
		print_indent(indent); \
		printf(str); \
	} while (0)

#define PRINT_FMT_INDENT(indent, fmt, ...)\
	do { \
		print_indent(indent); \
		printf(fmt, __VA_ARGS__); \
	} while (0)

static void print_indent(int amount)
{
	for (int i = 0; i < amount; i++) {
		printf(" ");
	}
}

static const char *unary_type_str[] = {
	[AST_UNARY_BITWISE_NOT] = "BITWISE NOT",
	[AST_UNARY_NEGATE] = "NEGATE",
	[AST_UNARY_LOGICAL_NOT] = "LOGICAL NOT",
};

static const char *binary_type_str[] = {
	[AST_BINARY_ADD] = "ADD",
	[AST_BINARY_SUBTRACT] = "SUBTRACT",
	[AST_BINARY_MULTIPLY] = "MULTIPLY",
	[AST_BINARY_DIVIDE] = "DIVIDE",
	[AST_BINARY_REMAINDER] = "REMAINDER",
	[AST_BINARY_BITWISE_AND] = "AND",
	[AST_BINARY_BITWISE_OR] = "OR",
	[AST_BINARY_BITWISE_XOR] = "XOR",
	[AST_BINARY_LEFT_SHIFT] = "LEFT SHIFT",
	[AST_BINARY_RIGHT_SHIFT] = "RIGHT SHIFT",
	[AST_BINARY_LOGICAL_AND] = "LOGICAL AND",
	[AST_BINARY_LOGICAL_OR] = "LOGICAL OR",
	[AST_BINARY_LOGICAL_EQUAL] = "LOGICAL EQUAL",
	[AST_BINARY_LOGICAL_NOT_EQUAL] = "LOGICAL NOT EQUAL",
	[AST_BINARY_LOGICAL_LESS_THAN] = "LOGICAL LESS THAN",
	[AST_BINARY_LOGICAL_LESS_EQUAL] = "LOGICAL LESS EQUAL",
	[AST_BINARY_LOGICAL_GREATER_THAN] = "LOGICAL GREATER THAN",
	[AST_BINARY_LOGICAL_GREATER_EQUAL] = "LOGICAL GREATER EQUAL",
};

static const char *statement_type_str[] = {
	[AST_STATEMENT_RETURN] = "RETURN",
	[AST_STATEMENT_EXPRESSION] = "EXPRESSION",
	[AST_STATEMENT_NULL_EXPRESSION] = "NULL EXPRESSION",
};

static ASTNode *alloc_node(ASTNodeType type)
{
	ASTNode *node = malloc(sizeof(ASTNode));
	node->type = type;
	return node;
}

void ast_print(const ASTNode *main, int indent)
{
	if (main == NULL) return;

	int next_indent = indent + INDENT_PER_LEVEL;

	switch(main->type) {
		case AST_PROGRAM:
			PRINT_INDENT(indent, "Program(\n");
			ast_print(main->node_value.program.function, next_indent);
			PRINT_INDENT(indent, ")\n");
			break;
		case AST_FUNCTION:
			PRINT_INDENT(indent, "Function(\n");
			PRINT_FMT_INDENT(next_indent, "name=%s\n", main->node_value.function.name);
			PRINT_INDENT(next_indent, "body=\n");
			ASTNode **body = main->node_value.function.body;
			for (int i = 0; i < array_length(body); i++) {
				ast_print(body[i], next_indent + INDENT_PER_LEVEL);
			}
			PRINT_INDENT(indent, ")\n");
			break;
		case AST_STATEMENT:{
			ASTStatementType type = main->node_value.statement.type;
			PRINT_INDENT(indent, "Statement(\n");
			PRINT_FMT_INDENT(next_indent, "type=%s\n", statement_type_str[type]);
			PRINT_INDENT(next_indent, "expression=\n");
			ast_print(main->node_value.statement.expression, next_indent + INDENT_PER_LEVEL);
			PRINT_INDENT(indent, ")\n");
			break;
		}
		case AST_DECLARATION:
			PRINT_INDENT(indent, "Declaration(\n");
			PRINT_FMT_INDENT(next_indent, "name=%s\n", main->node_value.declaration.name);
			PRINT_INDENT(next_indent, "init=\n");
			if (main->node_value.declaration.init != NULL){
				ast_print(main->node_value.declaration.init, next_indent + INDENT_PER_LEVEL);
			} else {
				PRINT_INDENT(next_indent + INDENT_PER_LEVEL, "NULL");
			}
			PRINT_INDENT(indent, ")\n");
			break;
		case AST_INT_LITERAL:
			PRINT_FMT_INDENT(indent, "INT_LIT(%d)\n", main->node_value.int_literal.value);
			break;
		case AST_UNARY:{
			const ASTUnaryType type = main->node_value.unary.type;
			PRINT_INDENT(indent, "Unary(\n");
			PRINT_FMT_INDENT(next_indent, "type=%s\n", unary_type_str[type]);
			PRINT_INDENT(next_indent, "expression=\n");
			ast_print(main->node_value.unary.expression, next_indent + INDENT_PER_LEVEL);
			PRINT_INDENT(indent, ")\n");
			break;
	    }
		case AST_BINARY:{
			const ASTBinaryType type = main->node_value.binary.type;
			PRINT_INDENT(indent, "Binary(\n");
			PRINT_FMT_INDENT(next_indent, "type=%s\n", binary_type_str[type]);
			PRINT_INDENT(next_indent, "left=\n");
			ast_print(main->node_value.binary.left, next_indent + INDENT_PER_LEVEL);
			PRINT_INDENT(next_indent, "right=\n");
			ast_print(main->node_value.binary.right, next_indent + INDENT_PER_LEVEL);
			PRINT_INDENT(indent, ")\n");
			break;
	    }
		case AST_VARIABLE:{
			PRINT_INDENT(indent, "Variable(\n");
			PRINT_FMT_INDENT(next_indent, "identifier=%s\n", main->node_value.variable.identifier);
			PRINT_INDENT(indent, ")\n");
			break;
	    }
		case AST_ASSIGNMENT:{
			PRINT_INDENT(indent, "Assignment(\n");
			PRINT_INDENT(next_indent, "left=\n");
			ast_print(main->node_value.assignment.left, next_indent + INDENT_PER_LEVEL);
			PRINT_INDENT(next_indent, "right=\n");
			ast_print(main->node_value.assignment.right, next_indent + INDENT_PER_LEVEL);
			PRINT_INDENT(indent, ")\n");
			break;
	    }

		default:
			PRINT_INDENT(indent, "UNKNOWN_TYPE");
			break;
	}
}

void ast_free_node(ASTNode *main)
{
	if (main == NULL) return;

	switch(main->type) {
		case AST_PROGRAM:
			ast_free_node(main->node_value.program.function);
			break;
		case AST_FUNCTION:{
			// Name pointer does not belong to the node, don't free it here
			ASTNode **array = main->node_value.function.body;
			for (int i = 0; i < array_length(array); i++) {
				ast_free_node(array[i]);
			}
			array_free(array);
			break;
		}
		case AST_STATEMENT:
			ast_free_node(main->node_value.statement.expression);
			break;
		case AST_DECLARATION:
			// Name pointer does not belong to the node, don't free it here
			ast_free_node(main->node_value.declaration.init);
			break;
		case AST_UNARY:
			ast_free_node(main->node_value.unary.expression);
			break;
		case AST_BINARY:
			ast_free_node(main->node_value.binary.left);
			ast_free_node(main->node_value.binary.right);
			break;
		case AST_VARIABLE:
			// Identifier pointer does not belong to the node, don't free it here
			break;
		case AST_ASSIGNMENT:
			ast_free_node(main->node_value.assignment.left);
			ast_free_node(main->node_value.assignment.right);
			break;

		default: break;
	}

	free(main);
}

ASTNode *ast_program(ASTNode *func)
{
	ASTNode *node = alloc_node(AST_PROGRAM);
	node->node_value.program.function = func;
	return node;
}

ASTNode *ast_function(const char *name, ASTNode **body)
{
	ASTNode *node = alloc_node(AST_FUNCTION);
	node->node_value.function.name = name;
	node->node_value.function.body = body;
	return node;
}

ASTNode *ast_statement(ASTStatementType type, ASTNode *exp)
{
	ASTNode *node = alloc_node(AST_STATEMENT);
	node->node_value.statement.type = type;
	node->node_value.statement.expression = exp;
	return node;
}

ASTNode *ast_declaration(const char *name, ASTNode *init)
{
	ASTNode *node = alloc_node(AST_DECLARATION);
	node->node_value.declaration.name = name;
	node->node_value.declaration.init = init;
	return node;
}

ASTNode *ast_int_literal(int value)
{
	ASTNode *node = alloc_node(AST_INT_LITERAL);
	node->node_value.int_literal.value = value;
	return node;
}

ASTNode *ast_unary(ASTUnaryType type, ASTNode *exp)
{
	ASTNode *node = alloc_node(AST_UNARY);
	node->node_value.unary.type = type;
	node->node_value.unary.expression = exp;
	return node;
}

ASTNode *ast_binary(ASTBinaryType type, ASTNode *left, ASTNode *right)
{
	ASTNode *node = alloc_node(AST_BINARY);
	node->node_value.binary.type = type;
	node->node_value.binary.left = left;
	node->node_value.binary.right = right;
	return node;
}

ASTNode *ast_variable(const char *identifier)
{
	ASTNode *node = alloc_node(AST_VARIABLE);
	node->node_value.variable.identifier = identifier;
	return node;
}

ASTNode *ast_assignment(ASTNode *left, ASTNode *right)
{
	ASTNode *node = alloc_node(AST_ASSIGNMENT);
	node->node_value.assignment.left = left;
	node->node_value.assignment.right = right;
	return node;
}
