#ifndef UNEBCC_AST_H
#define UNEBCC_AST_H

typedef	enum ASTNodeType {
	AST_PROGRAM,
	AST_FUNCTION,
	AST_RETURN_STATEMENT,
	AST_INT_LITERAL,
}ASTNodeType;

typedef struct ASTNode ASTNode;
struct ASTNode {
	ASTNodeType type;

	union {
		struct {
			ASTNode *function;
		} program;

		struct {
			const char *name;
			ASTNode *body;
		} function;

		struct {
			ASTNode *expression;
		} return_statement;

		struct {
			int value;
		} int_literal;
	} node_value;
};

// Prints the AST in an understandable way
void ast_print(ASTNode *main, int indent);

// Frees all child nodes before the main node
void ast_free_node(ASTNode *main);

// Allocates a new program node
ASTNode *ast_program(ASTNode *func);
// Allocates a new function node
ASTNode *ast_function(const char *name, ASTNode *body);
// Allocates a new return statement node
ASTNode *ast_return_statement(ASTNode *exp);
// Allocates a new int literal node
ASTNode *ast_int_literal(int value);

#endif // UNEBCC_AST_H
