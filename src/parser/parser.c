#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "lexer/token.h"
#include "parser/parser.h"

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


void parser_init(Parser *p, Token *token_array)
{
	assert(token_array != NULL);

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
	assert(p->ast == NULL);
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
	assert(p->next_token > 0);

	return &p->tokens[p->next_token - 1];
}

static Token* peek(Parser *p)
{
	return &p->tokens[p->next_token];
}

static ASTNode *parse_program(Parser *p)
{
	assert(p->ast == NULL);

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
	if (expect(p, TOKEN_INTEGER_LITERAL) == false) return NULL;

	Token *prev = previous(p);
	int value = atoi(prev->literal);

	return ast_int_literal(value);
}
