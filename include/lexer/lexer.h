#ifndef UNEBCC_LEXER_LEXER_H
#define UNEBCC_LEXER_LEXER_H

#include <stdbool.h>

#include "token.h"

// Initializes the lexer with a specific buffer
void lexer_init(const char *buffer, unsigned int buffer_len);
// Frees the lexer's token array
void lexer_deinit(void);
// Scans all tokens and adds them to the array
void lexer_scan_tokens(void);
// Returns the array of tokens (call after lexer_scan_tokens);
Token* lexer_get_token_array(void);
// Returns whether the lexer had an error or not
bool lexer_had_error(void);

#endif // UNEBCC_LEXER_LEXER_H
