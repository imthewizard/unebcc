#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lexer/token.h"
#include "parser/ast.h"
#include "parser/parser.h"

#include "utils/debug.h"

static ASTNode *parse_program(Parser *p);
static ASTNode *parse_function(Parser *p);
static ASTNode *parse_statement(Parser *p);
static ASTNode *parse_expression(Parser *p);

// Checks if the next token is of the expected type and advances.
// If it isn't, an error message will be printed.
static bool expect(Parser *p, TokenType token_type);
// Returns the most recently consumed token
static Token* previous(Parser *p);
// Returns the next token
static Token* peek(Parser *p);
// Advances the parser
static void advance(Parser *p);


void parser_init(Parser *p, Token *token_array)
{
	ASSERT(token_array != NULL, "token_array must be non-null, use the lexer first");

	p->tokens = token_array;
	p->next_token = 0;
	p->ast = NULL;
}

void parser_deinit(Parser *p)
{
	if (p->ast != NULL) {
		ast_free_node(p->ast);
	}
}

void parser_parse(Parser *p)
{
	ASSERT(p->ast == NULL, "ast is not null, create a new parser");
	p->ast = parse_program(p);
}

static bool expect(Parser *p, TokenType token_type)
{
	if (p->tokens[p->next_token++].type == token_type) {
		return true;
	}
	fprintf(stderr, "Syntax error\n");
	return false;
}

static Token* previous(Parser *p)
{
	ASSERT(p->next_token > 0, "no previous token");

	return &p->tokens[p->next_token - 1];
}

static Token* peek(Parser *p)
{
	return &p->tokens[p->next_token];
}

static void advance(Parser *p)
{
	p->next_token++;
}

static ASTNode *parse_program(Parser *p)
{
	ASSERT(p->ast == NULL, "ast is not null, create a new parser");

	ASTNode *function = parse_function(p);
	if (function == NULL) return NULL;

	return ast_program(function);
}

static ASTNode *parse_function(Parser *p)
{
	if (expect(p, TOKEN_INT) == false) return NULL;
	if (expect(p, TOKEN_IDENTIFIER) == false) return NULL;

	char *name = previous(p)->literal;

	if (expect(p, TOKEN_LPAREN) == false) return NULL;
	if (expect(p, TOKEN_VOID) == false) return NULL;
	if (expect(p, TOKEN_RPAREN) == false) return NULL;

	if (expect(p, TOKEN_LBRACE) == false) return NULL;

	ASTNode *statement = parse_statement(p);
	if (statement == NULL) return NULL;

	if (expect(p, TOKEN_RBRACE) == false) return NULL;

	return ast_function(name, statement);
}

static ASTNode *parse_statement(Parser *p)
{
	if (expect(p, TOKEN_RETURN) == false) return NULL;

	ASTNode *exp = parse_expression(p);
	if (exp == NULL) return NULL;

	if (expect(p, TOKEN_SEMICOLON) == false) return NULL;

	return ast_return_statement(exp);
}

static ASTNode *parse_expression(Parser *p)
{
	const Token *next = peek(p);

	switch(next->type) {
		case TOKEN_INTEGER_LITERAL:
			if (expect(p, TOKEN_INTEGER_LITERAL) == false) return NULL;
			Token *prev = previous(p);
			int value = atoi(prev->literal);
			return ast_int_literal(value);
		case TOKEN_TILDE:{
			advance(p);
			ASTNode *inner_exp = parse_expression(p);
			return ast_unary(AST_UNARY_BITWISE_NOT, inner_exp);
		}
		case TOKEN_MINUS:{
			advance(p);
			ASTNode *inner_exp = parse_expression(p);
			return ast_unary(AST_UNARY_NEGATE, inner_exp);
		}
		case TOKEN_LPAREN:
			advance(p);
			ASTNode *inner_exp = parse_expression(p);
			if (expect(p, TOKEN_RPAREN) == false) return NULL;
			return inner_exp;

		default:
			fprintf(stderr, "Malformed expression\n");
			return NULL;
	}
}
