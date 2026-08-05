#ifndef UNEBCC_PARSER_H
#define UNEBCC_PARSER_H

#include "ast.h"
#include "token.h"

typedef struct Parser {
	Token *tokens;
	unsigned int next_token;

	ASTNode *ast;
}Parser;

// Initializes a parser with the specified token array
void parser_init(Parser *p, Token *token_array);
// Deinitializes the parser
void parser_deinit(Parser *p);
// Parse everything
void parser_parse(Parser *p);

#endif // UNEBCC_PARSER_H
