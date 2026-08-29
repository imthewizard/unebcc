#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/ast.h"

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
};

static const char *binary_type_str[] = {
	[AST_BINARY_ADD] = "ADD",
	[AST_BINARY_SUBTRACT] = "SUBTRACT",
	[AST_BINARY_MULTIPLY] = "MULTIPLY",
	[AST_BINARY_DIVIDE] = "DIVIDE",
	[AST_BINARY_REMAINDER] = "REMAINDER",
};

static ASTNode *alloc_node(ASTNodeType type)
{
	ASTNode *node = malloc(sizeof(ASTNode));
	node->type = type;
	return node;
}

void ast_print(ASTNode *main, int indent)
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
			ast_print(main->node_value.function.body, next_indent + INDENT_PER_LEVEL);
			PRINT_INDENT(indent, ")\n");
			break;
		case AST_RETURN_STATEMENT:
			PRINT_INDENT(indent, "Return(\n");
			ast_print(main->node_value.return_statement.expression, next_indent + INDENT_PER_LEVEL);
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
		case AST_FUNCTION:
			// Since the name pointer does not belong to the node, we won't free it here
			ast_free_node(main->node_value.function.body);
			break;
		case AST_RETURN_STATEMENT:
			ast_free_node(main->node_value.return_statement.expression);
			break;
		case AST_UNARY:
			ast_free_node(main->node_value.unary.expression);
			break;
		case AST_BINARY:
			ast_free_node(main->node_value.binary.left);
			ast_free_node(main->node_value.binary.right);
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

ASTNode *ast_function(const char *name, ASTNode *body)
{
	ASTNode *node = alloc_node(AST_FUNCTION);
	node->node_value.function.name = name;
	node->node_value.function.body = body;
	return node;
}

ASTNode *ast_return_statement(ASTNode *exp)
{
	ASTNode *node = alloc_node(AST_RETURN_STATEMENT);
	node->node_value.return_statement.expression = exp;
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
