#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "semantic/semantic.h"
#include "parser/ast.h"
#include "utils/array.h"
#include "utils/stringmap.h"
#include "utils/debug.h"

// Prints an error message
static void fail(const char *msg);
// Generates an unique name from a name. Return value must be freed later
static char *generate_unique_name(const char *name);

static bool resolve_statement(ASTNode *statement_node, StringMap *variable_map);
static bool resolve_declaration(ASTNode *decl_node, StringMap *variable_map);
static bool resolve_expression(ASTNode *expr_node, StringMap *variable_map);

// This is a bit hacky
char **allocated_unique_names = NULL;

bool semantic_analysis(ASTNode *ast_main)
{
	allocated_unique_names = array_create(allocated_unique_names, 1);
	StringMap variable_map = string_map_init();

	ASTNode *main_fn = ast_main->node_value.program.function;

	bool ok = true;

	for (int i = 0; i < array_length(main_fn->node_value.function.body); i++) {
		ASTNode *current = main_fn->node_value.function.body[i];

		switch (current->type) {
			case AST_DECLARATION:
				if (!resolve_declaration(current, &variable_map))
					ok = false;
				break;
			case AST_STATEMENT:
				if (!resolve_statement(current, &variable_map))
					ok = false;
				break;
			case AST_ASSIGNMENT:
				if (!resolve_expression(current, &variable_map))
					ok = false;
				break;
			case AST_BINARY:
				if (!resolve_expression(current, &variable_map))
					ok = false;
				break;
			default: continue;
		}
	}

	string_map_deinit(&variable_map);

	return ok;
}

void semantic_free_allocated(void)
{
	for (int i = 0; i < array_length(allocated_unique_names); i++) {
		free(allocated_unique_names[i]);
	}
}

static void fail(const char *msg)
{
	printf("Semantic analysis error: %s", msg);
}

static char *generate_unique_name(const char *name)
{
	// TODO: make this better
	static int counter = 0;
	// char *
	int name_len = strlen(name);
	char *unique_str = malloc(sizeof(char) * (name_len + 10));
	sprintf(unique_str, "%s_sa%d", name, counter++);

	array_push(allocated_unique_names, unique_str);

	return unique_str;
}

static bool resolve_statement(ASTNode *statement_node, StringMap *variable_map)
{
	ASSERT(statement_node->type == AST_STATEMENT, "invalid node type");
	ASTNode *expr = statement_node->node_value.statement.expression;
	switch (statement_node->node_value.statement.type) {
		case AST_STATEMENT_RETURN:
			return resolve_expression(expr, variable_map);
		case AST_STATEMENT_EXPRESSION:
			return resolve_expression(expr, variable_map);
		case AST_STATEMENT_NULL_EXPRESSION:
			return true;
		default: UNIMPLEMENTED("unhandled type");
	}
}

static bool resolve_declaration(ASTNode *decl_node, StringMap *variable_map)
{
	ASSERT(decl_node->type == AST_DECLARATION, "invalid node type");

	const char **name = &decl_node->node_value.declaration.name;
	ASTNode *init = decl_node->node_value.declaration.init;

	if (string_map_get(variable_map, *name) != NULL) {
		fail("Duplicate variable declaration");
		return false;
	}

	char *unique = generate_unique_name(*name);
	string_map_put(variable_map, *name, unique);
	*name = unique;

	if (init != NULL) {
		return resolve_expression(init, variable_map);
	}

	return true;
}

static bool resolve_expression(ASTNode *expr_node, StringMap *variable_map)
{
	switch (expr_node->type) {
		case AST_ASSIGNMENT:{
			ASTNode *left = expr_node->node_value.assignment.left;
			ASTNode *right = expr_node->node_value.assignment.right;
			if (left->type != AST_VARIABLE) {
				fail("Invalid lvalue");
				return false;
			}
			bool ok_l = resolve_expression(left, variable_map);
			bool ok_r = resolve_expression(right, variable_map);
			return (ok_l && ok_r);
		}
		case AST_VARIABLE:{
			const char **identifier = &expr_node->node_value.variable.identifier;
			const char *possible_unique_id = string_map_get(variable_map, *identifier);
			if (possible_unique_id != NULL) {
				*identifier = possible_unique_id;
				return true;
			} else {
				fail("Undeclared variable");
				return false;
			}
		}
		case AST_UNARY:{
			return resolve_expression(expr_node->node_value.unary.expression, variable_map);
	    }
		case AST_BINARY:{
			ASTNode *left = expr_node->node_value.binary.left;
			ASTNode *right = expr_node->node_value.binary.right;
			bool ok_l = resolve_expression(left, variable_map);
			bool ok_r = resolve_expression(right, variable_map);
			return (ok_l && ok_r);
	    }

		// Treat everything else as ok
		default: return true;
	}
}
