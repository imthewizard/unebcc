#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "lexer/lexer.h"
#include "utils/array.h"
#include "utils/debug.h"

// Increments lexer->pos to skip whitespace, if needed
static void skip_whitespace(Lexer *lexer);
// Checks if there are more chars to be read
static bool is_at_end(Lexer *lexer);
// Returns the next character in the buffer and advance
static char consume(Lexer *lexer);
// Returns the next character in the buffer, does not advance
static char peek(Lexer *lexer);
// Advances to the next char
static void advance(Lexer *lexer);
// Scans the string buffer and return a single token
static Token scan_next_token(Lexer *lexer);

static Token handle_number(Lexer *lexer, unsigned int start_pos);
static Token handle_keyword_identifier(Lexer *lexer, unsigned int start_pos);

void lexer_init(Lexer *lexer, const char *buffer, unsigned int buffer_len)
{
	lexer->buffer = buffer;
	lexer->len = buffer_len;
	lexer->next_pos = 0;
}

#include <stdio.h>
void lexer_scan_tokens(Lexer *lexer, Token **token_array)
{
	ASSERT(lexer->buffer != NULL, "lexer buffer is invalid");
	ASSERT(*token_array != NULL, "token_array is invalid");

	Token token;
	do {
		token = scan_next_token(lexer);
		array_push(*token_array, token);
	} while (token.type != TOKEN_EOF);
}

static Token scan_next_token(Lexer *lexer)
{
	// Skip indentation/spaces
	skip_whitespace(lexer);

	if (is_at_end(lexer)) {
		return (Token){TOKEN_EOF, NULL};
	}

	unsigned int start_position = lexer->next_pos;
	char c = consume(lexer);

	// Single characters
	switch(c) {
		case('~'): return (Token){TOKEN_TILDE, NULL};
		case('-'):
		   if (peek(lexer) != '-')
			   return (Token){TOKEN_MINUS, NULL};
		   break;

		case('('): return (Token){TOKEN_LPAREN, NULL};
		case(')'): return (Token){TOKEN_RPAREN, NULL};
		case('{'): return (Token){TOKEN_LBRACE, NULL};
		case('}'): return (Token){TOKEN_RBRACE, NULL};
		case(';'): return (Token){TOKEN_SEMICOLON, NULL};
	}

	// Double characters (TODO: maybe put these inside the single chars switch)
	switch(c) {
		case('-'): return (Token){TOKEN_DECREMENT, NULL};
	}

	// Others
	if (isdigit(c)) {
		return handle_number(lexer, start_position);
	} else if ((isalpha(c)) || c == '_') {
		return handle_keyword_identifier(lexer, start_position);
	}

	return (Token){TOKEN_INVALID, NULL};
}

static void skip_whitespace(Lexer *lexer)
{
	while(isspace(lexer->buffer[lexer->next_pos])){
		lexer->next_pos++;
	}
}

static bool is_at_end(Lexer *lexer)
{
	return (peek(lexer) == '\0');
}

static char consume(Lexer *lexer)
{
	return lexer->buffer[lexer->next_pos++];
}

static char peek(Lexer *lexer)
{
	return lexer->buffer[lexer->next_pos];
}

static void advance(Lexer *lexer)
{
	lexer->next_pos++;
}

static Token handle_number(Lexer *lexer, unsigned int start_pos)
{
	// Handling integers constants
	while (isdigit(peek(lexer))){
		advance(lexer);
	}

	unsigned int len = lexer->next_pos - start_pos;
	char *buf = malloc(sizeof(char) * (len + 1));
	strncpy(buf, &(lexer->buffer[start_pos]), len);
	buf[len] = '\0';

	return (Token){TOKEN_INTEGER_LITERAL, buf};
}

static Token handle_keyword_identifier(Lexer *lexer, unsigned int start_pos)
{
	while (isalnum(peek(lexer)) || peek(lexer) == '_') {
		advance(lexer);
	}

	unsigned int len = lexer->next_pos - start_pos;
	char *buf = malloc(sizeof(char) * (len + 1));
	strncpy(buf, &(lexer->buffer[start_pos]), len);
	buf[len] = '\0';

	// Check if it's a keyword or an identifier
	TokenType equivalent_type = keyword_to_tokentype(buf);
	if (equivalent_type != TOKEN_INVALID) {
		// Keyword
		free(buf);
		return (Token){equivalent_type, NULL};
	} else {
		// Identifier
		return (Token){TOKEN_IDENTIFIER, buf};
	}
}
