#ifndef UNEBCC_PARSER_H
#define UNEBCC_PARSER_H

#include <stdbool.h>

#include "ast.h"
#include "lexer/token.h"

typedef struct Parser {
	const Token *tokens;
	unsigned int next_token;

	ASTNode *ast;

	bool had_error;
}Parser;

// Initializes a parser with the specified token array
void parser_init(Parser *p, const Token *token_array);
// Deinitializes the parser
void parser_deinit(Parser *p);
// Parse everything
void parser_parse(Parser *p);

#endif // UNEBCC_PARSER_H
