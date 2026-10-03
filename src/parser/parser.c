#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lexer/token.h"
#include "parser/ast.h"
#include "parser/parser.h"

#include "utils/debug.h"
#include "utils/array.h"

typedef struct Parser {
	const Token *tokens;
	unsigned int next_token;

	ASTNode *ast;

	bool had_error;
}Parser;
Parser *parser = NULL;

static ASTNode *parse_program(void);
static ASTNode *parse_function(void);
static ASTNode *parse_block_item(void);
static ASTNode *parse_declaration(void);
static ASTNode *parse_statement(void);
static ASTNode *parse_expression(int min_prec);
static ASTNode *parse_factor(void);

// Checks if the next token is of the expected type and advances.
// If it isn't, an error message will be printed.
static bool expect(TokenType expected_type);
// Returns the most recently consumed token
static const Token* previous(void);
// Returns the next token
static const Token* peek(void);
// Advances the parser
static void advance(void);
// Gets the precedence of a binary operator token
static int precedence(TokenType token_type);
// Returns true if the token is a binary operator
static bool is_binary_operator(TokenType token_type);


void parser_init(const Token *token_array)
{
	ASSERT(parser == NULL, "parser was already initialized");
	parser = malloc(sizeof(Parser));

	ASSERT(token_array != NULL, "token_array must be non-null, use the lexer first");

	parser->tokens = token_array;
	parser->next_token = 0;
	parser->ast = NULL;
	parser->had_error = false;
}

void parser_deinit(void)
{
	if (parser != NULL) {
		if (parser->ast != NULL) {
			ast_free_node(parser->ast);
		}
		free(parser);
	}
}

void parser_parse(void)
{
	ASSERT(parser->ast == NULL, "ast is not null, create a new parser");
	parser->ast = parse_program();
}

ASTNode *parser_get_ast(void)
{
	return parser->ast;
}

bool parser_had_error(void)
{
	return parser->had_error;
}

static bool expect(TokenType expected_type)
{
	TokenType next_type = parser->tokens[parser->next_token++].type;
	if (next_type == expected_type) {
		return true;
	}
	fprintf(stderr, "Syntax error: ");
	fprintf(stderr, "expected %s, ", str_token_type(expected_type));
	fprintf(stderr, "got %s\n", str_token_type(next_type));
	parser->had_error = true;
	return false;
}

static const Token* previous(void)
{
	ASSERT(parser->next_token > 0, "no previous token");

	return &parser->tokens[parser->next_token - 1];
}

static const Token* peek(void)
{
	return &parser->tokens[parser->next_token];
}

static void advance(void)
{
	parser->next_token++;
}

static int precedence(TokenType token_type)
{
	switch (token_type) {
		case TOKEN_EQUAL: return 1;

		case TOKEN_LOGICAL_OR: return 5;

		case TOKEN_LOGICAL_AND: return 10;

		case TOKEN_PIPE: return 11;
		case TOKEN_CARET: return 12;
		case TOKEN_AMPERSAND: return 13;

		case TOKEN_NOT_EQUAL:
		case TOKEN_LOGICAL_EQUAL: return 30;

		case TOKEN_GREATER_EQUAL:
		case TOKEN_GREATER_THAN:
		case TOKEN_LESS_EQUAL:
		case TOKEN_LESS_THAN: return 35;

		case TOKEN_LEFT_SHIFT:
		case TOKEN_RIGHT_SHIFT: return 40;

		case TOKEN_MINUS:
		case TOKEN_PLUS: return 45;

		case TOKEN_PERCENT:
		case TOKEN_FORWARD_SLASH:
		case TOKEN_ASTERISK: return 50;

		default: UNIMPLEMENTED("unhandled token type in precedence");
	}
}

static bool is_binary_operator(TokenType token_type)
{
	switch (token_type) {
		case TOKEN_PLUS:
		case TOKEN_MINUS:
		case TOKEN_ASTERISK:
		case TOKEN_FORWARD_SLASH:
		case TOKEN_PERCENT:
		case TOKEN_AMPERSAND:
		case TOKEN_PIPE:
		case TOKEN_CARET:
		case TOKEN_LEFT_SHIFT:
		case TOKEN_RIGHT_SHIFT:
		case TOKEN_LESS_THAN:
		case TOKEN_GREATER_THAN:
		case TOKEN_LOGICAL_AND:
		case TOKEN_LOGICAL_OR:
		case TOKEN_LOGICAL_EQUAL:
		case TOKEN_NOT_EQUAL:
		case TOKEN_LESS_EQUAL:
		case TOKEN_GREATER_EQUAL:
		case TOKEN_EQUAL:
			return true;

		default: return false;
	}
}

static ASTNode *parse_program(void)
{
	ASSERT(parser->ast == NULL, "ast is not null, create a new parser");

	ASTNode *function = parse_function();
	if (function == NULL) return NULL;

	if (expect(TOKEN_EOF) == false) return NULL;

	return ast_program(function);
}

static ASTNode *parse_function(void)
{
	if (expect(TOKEN_INT) == false) return NULL;
	if (expect(TOKEN_IDENTIFIER) == false) return NULL;

	char *name = previous()->literal;

	if (expect(TOKEN_LPAREN) == false) return NULL;
	if (expect(TOKEN_VOID) == false) return NULL;
	if (expect(TOKEN_RPAREN) == false) return NULL;

	if (expect(TOKEN_LBRACE) == false) return NULL;

	ASTNode **block_items = array_create(block_items, 1);
	ASTNode *current_block_item;
	while (peek()->type != TOKEN_RBRACE) {
		current_block_item = parse_block_item();
		if (current_block_item == NULL) return NULL;

		array_push(block_items, current_block_item);
	}

	if (expect(TOKEN_RBRACE) == false) return NULL;
	return ast_function(name, block_items);
}

static ASTNode *parse_block_item(void)
{
	if (peek()->type == TOKEN_INT) {
		return parse_declaration();
	} else {
		return parse_statement();
	}
}

static ASTNode *parse_declaration(void)
{
	if (expect(TOKEN_INT) == false) return NULL;

	const Token *possible_identifier = peek();
	if (expect(TOKEN_IDENTIFIER) == false) return NULL;

	ASTNode *exp = NULL;
	if (peek()->type == TOKEN_EQUAL) {
		advance();

		exp = parse_expression(0);
		if (exp == NULL) return NULL;
	}

	if (expect(TOKEN_SEMICOLON) == false) return NULL;

	return ast_declaration(possible_identifier->literal, exp);
}

static ASTNode *parse_statement(void)
{
	const Token *next = peek();

	if (next->type == TOKEN_RETURN) {
		advance();

		ASTNode *exp = parse_expression(0);
		if (exp == NULL) return NULL;

		if (expect(TOKEN_SEMICOLON) == false) return NULL;

		return ast_statement(AST_STATEMENT_RETURN, exp);
	}

	if (next->type == TOKEN_SEMICOLON) {
		advance();
		return ast_statement(AST_STATEMENT_NULL_EXPRESSION, NULL);
	}

	ASTNode *exp = parse_expression(0);

	if (expect(TOKEN_SEMICOLON) == false) return NULL;

	return exp;
}

static ASTNode *parse_expression(int min_prec)
{
	ASTNode *left = parse_factor();
	const Token *next = peek();

	while (1) {
		if (!is_binary_operator(next->type)) return left;

		int prec = precedence(next->type);
		if (prec < min_prec) return left;

		advance();

		if (next->type == TOKEN_EQUAL) {
			ASTNode *right = parse_expression( prec);
			return ast_assignment(left, right);
		}

		ASTNode *right = parse_expression(prec + 1);
		ASTBinaryType bin_type;
		switch (next->type) {
			case TOKEN_PLUS: bin_type = AST_BINARY_ADD; break;
			case TOKEN_MINUS: bin_type = AST_BINARY_SUBTRACT; break;
			case TOKEN_ASTERISK: bin_type = AST_BINARY_MULTIPLY; break;
			case TOKEN_FORWARD_SLASH: bin_type = AST_BINARY_DIVIDE; break;
			case TOKEN_PERCENT: bin_type = AST_BINARY_REMAINDER; break;
			case TOKEN_AMPERSAND: bin_type = AST_BINARY_BITWISE_AND; break;
			case TOKEN_PIPE: bin_type = AST_BINARY_BITWISE_OR; break;
			case TOKEN_CARET: bin_type = AST_BINARY_BITWISE_XOR; break;
			case TOKEN_LEFT_SHIFT: bin_type = AST_BINARY_LEFT_SHIFT; break;
			case TOKEN_RIGHT_SHIFT: bin_type = AST_BINARY_RIGHT_SHIFT; break;
			case TOKEN_LOGICAL_AND: bin_type = AST_BINARY_LOGICAL_AND; break;
			case TOKEN_LOGICAL_OR: bin_type = AST_BINARY_LOGICAL_OR; break;
			case TOKEN_LOGICAL_EQUAL: bin_type = AST_BINARY_LOGICAL_EQUAL; break;
			case TOKEN_NOT_EQUAL: bin_type = AST_BINARY_LOGICAL_NOT_EQUAL; break;
			case TOKEN_LESS_THAN: bin_type = AST_BINARY_LOGICAL_LESS_THAN; break;
			case TOKEN_LESS_EQUAL: bin_type = AST_BINARY_LOGICAL_LESS_EQUAL; break;
			case TOKEN_GREATER_THAN: bin_type = AST_BINARY_LOGICAL_GREATER_THAN; break;
			case TOKEN_GREATER_EQUAL: bin_type = AST_BINARY_LOGICAL_GREATER_EQUAL; break;
			default: UNIMPLEMENTED("unhandled token case in parse_expression");
		}

		left = ast_binary(bin_type, left, right);
		next = peek();
	}
	return left;
}

static ASTNode *parse_factor(void)
{
	const Token *next = peek();

	switch(next->type) {
		case TOKEN_INTEGER_LITERAL:{
			advance();
			int value = atoi(next->literal);
			return ast_int_literal(value);
		}
		case TOKEN_IDENTIFIER:{
			advance();
			const char *identifier = next->literal;
			return ast_variable(identifier);
		}

		case TOKEN_TILDE:{
			advance();
			ASTNode *inner_exp = parse_factor();
			return ast_unary(AST_UNARY_BITWISE_NOT, inner_exp);
		}
		case TOKEN_MINUS:{
			advance();
			ASTNode *inner_exp = parse_factor();
			return ast_unary(AST_UNARY_NEGATE, inner_exp);
		}
		case TOKEN_EXCLAMATION:{
			advance();
			ASTNode *inner_exp = parse_factor();
			return ast_unary(AST_UNARY_LOGICAL_NOT, inner_exp);
		}

		case TOKEN_LPAREN:
			advance();
			ASTNode *inner_exp = parse_expression(0);
			if (expect(TOKEN_RPAREN) == false) return NULL;
			return inner_exp;

		default:
			fprintf(stderr, "Malformed factor\n");
			parser->had_error = true;
			return NULL;
	}
}
