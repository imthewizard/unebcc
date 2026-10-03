#ifndef UNEBCC_PARSER_PARSER_H
#define UNEBCC_PARSER_PARSER_H

#include <stdbool.h>

#include "ast.h"
#include "lexer/token.h"

// Initializes a parser with the specified token array
void parser_init(const Token *token_array);
// Deinitializes the parser
void parser_deinit(void);
// Parse everything
void parser_parse(void);
// Returns the AST (call after parser_parse)
ASTNode *parser_get_ast(void);
// Returns whether the parser had an error or not
bool parser_had_error(void);


#endif // UNEBCC_PARSER_PARSER_H
