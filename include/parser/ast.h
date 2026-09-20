#ifndef UNEBCC_AST_H
#define UNEBCC_AST_H

typedef	enum ASTNodeType {
	AST_PROGRAM,
	AST_FUNCTION,
	AST_STATEMENT,
	AST_DECLARATION,

	AST_INT_LITERAL,
	AST_UNARY,
	AST_BINARY,
	AST_VARIABLE,
	AST_ASSIGNMENT,
}ASTNodeType;

typedef enum ASTUnaryType {
	AST_UNARY_BITWISE_NOT,
	AST_UNARY_NEGATE,
	AST_UNARY_LOGICAL_NOT,
}ASTUnaryType;

typedef enum ASTBinaryType {
	AST_BINARY_ADD,
	AST_BINARY_SUBTRACT,
	AST_BINARY_MULTIPLY,
	AST_BINARY_DIVIDE,
	AST_BINARY_REMAINDER,
	AST_BINARY_BITWISE_AND,
	AST_BINARY_BITWISE_OR,
	AST_BINARY_BITWISE_XOR,
	AST_BINARY_LEFT_SHIFT,
	AST_BINARY_RIGHT_SHIFT,
	AST_BINARY_LOGICAL_AND,
	AST_BINARY_LOGICAL_OR,
	AST_BINARY_LOGICAL_EQUAL,
	AST_BINARY_LOGICAL_NOT_EQUAL,
	AST_BINARY_LOGICAL_LESS_THAN,
	AST_BINARY_LOGICAL_LESS_EQUAL,
	AST_BINARY_LOGICAL_GREATER_THAN,
	AST_BINARY_LOGICAL_GREATER_EQUAL,
}ASTBinaryType;

typedef enum ASTStatementType {
	AST_STATEMENT_RETURN,
	AST_STATEMENT_EXPRESSION,
	AST_STATEMENT_NULL_EXPRESSION,
} ASTStatementType;

typedef struct ASTNode ASTNode;
struct ASTNode {
	ASTNodeType type;

	union {
		struct {
			ASTNode *function;
		} program;

		struct {
			const char *name;
			// Array of memory allocated ASTNodes
			ASTNode **body;
		} function;

		struct {
			ASTStatementType type;
			ASTNode *expression;
		} statement;

		struct {
			const char *name;
			ASTNode *init;
		} declaration;

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

		struct {
			const char *identifier;
		} variable;

		struct {
			ASTNode *left;
			ASTNode *right;
		} assignment;
	} node_value;
};

// Prints the AST in an understandable way
void ast_print(const ASTNode *main, int indent);

// Frees all child nodes before the main node
void ast_free_node(ASTNode *main);

// Allocates a new program node
ASTNode *ast_program(ASTNode *func);
// Allocates a new function node. 'body' must be an array of allocated ASTNodes
ASTNode *ast_function(const char *name, ASTNode **body);
// Allocates a new statement node
ASTNode *ast_statement(ASTStatementType type, ASTNode *exp);
// Allocates a new declaration node
ASTNode *ast_declaration(const char *name, ASTNode *init);
// Allocates a new int literal node
ASTNode *ast_int_literal(int value);
// Allocates a new unary node
ASTNode *ast_unary(ASTUnaryType type, ASTNode *exp);
// Allocates a new binary node
ASTNode *ast_binary(ASTBinaryType type, ASTNode *left, ASTNode *right);
// Allocates a new variable node
ASTNode *ast_variable(const char *identifier);
// Allocates a new assignment node
ASTNode *ast_assignment(ASTNode *left, ASTNode *right);

#endif // UNEBCC_AST_H
