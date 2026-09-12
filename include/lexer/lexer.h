#ifndef UNEBCC_LEXER_H
#define UNEBCC_LEXER_H

#include <stdbool.h>

#include "token.h"

typedef struct Lexer{
	Token *token_array;

	// File buffer;
	const char *buffer;
	// Length of the buffer
	unsigned int len;
	// Position of the next character in the buffer
	unsigned int next_pos;

	bool had_error;
}Lexer;

// Initializes the lexer with a specific buffer
void lexer_init(Lexer *lexer, const char *buffer, unsigned int buffer_len);
// Frees the lexer's token array
void lexer_deinit(Lexer *lexer);
// Scans all tokens and adds them to the array
void lexer_scan_tokens(Lexer *lexer);

#endif // UNEBCC_LEXER_H
