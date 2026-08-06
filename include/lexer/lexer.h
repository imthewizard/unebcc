#ifndef UNEBCC_LEXER_H
#define UNEBCC_LEXER_H

#include "token.h"

typedef struct Lexer{
	// File buffer;
	const char *buffer;
	// Length of the buffer
	unsigned int len;
	// Position of the next character in the buffer
	unsigned int next_pos;
}Lexer;

// Initializes the lexer with a specific buffer
void lexer_init(Lexer *lexer, const char *buffer, unsigned int buffer_len);
// Scans all tokens and adds them to the array
void lexer_scan_tokens(Lexer *lexer, Token **token_array);

#endif // UNEBCC_LEXER_H
