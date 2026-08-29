#ifndef UNEBCC_AST_H
#define UNEBCC_AST_H

typedef	enum ASTNodeType {
	AST_PROGRAM,
	AST_FUNCTION,
	AST_RETURN_STATEMENT,
	AST_INT_LITERAL,
	AST_UNARY,
	AST_BINARY,
}ASTNodeType;

typedef enum ASTUnaryType {
	AST_UNARY_BITWISE_NOT,
	AST_UNARY_NEGATE,
}ASTUnaryType;

typedef enum ASTBinaryType {
	AST_BINARY_ADD,
	AST_BINARY_SUBTRACT,
	AST_BINARY_MULTIPLY,
	AST_BINARY_DIVIDE,
	AST_BINARY_REMAINDER,
}ASTBinaryType;

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

		struct {
			ASTUnaryType type;
			ASTNode *expression;
		} unary;

		struct {
			ASTBinaryType type;
			ASTNode *left;
			ASTNode *right;
		} binary;
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
// Allocates a new unary node
ASTNode *ast_unary(ASTUnaryType type, ASTNode *exp);
// Allocates a new binary node
ASTNode *ast_binary(ASTBinaryType type, ASTNode *left, ASTNode *right);

#endif // UNEBCC_AST_H
