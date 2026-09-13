#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "utils/array.h"
#include "utils/debug.h"

// Sets had_error and prints an error
#define ERROR(lexer, msg) \
	do { \
		printf("Lexer error: %s", msg); \
		lexer->had_error = true; \
	} while (0)

// Increments lexer->pos to skip whitespace, if needed
static void skip_whitespace(Lexer *lexer);
// Increments lexer->pos to skip comments, if needed
static void skip_comments(Lexer *lexer);
// Checks if there are more chars to be read
static bool is_at_end(const Lexer *lexer);
// Returns the next character in the buffer and advance
static char consume(Lexer *lexer);
// Returns the next character in the buffer, does not advance
static char peek(const Lexer *lexer);
// Advances to the next char
static void advance(Lexer *lexer);
// Scans the string buffer and return a single token
static Token scan_next_token(Lexer *lexer);

static Token handle_number(Lexer *lexer, unsigned int start_pos);
static Token handle_keyword_identifier(Lexer *lexer, unsigned int start_pos);

void lexer_init(Lexer *lexer, const char *buffer, unsigned int buffer_len)
{
	lexer->token_array = array_create(lexer->token_array, 16);
	lexer->buffer = buffer;
	lexer->len = buffer_len;
	lexer->next_pos = 0;
	lexer->had_error = false;
}

void lexer_deinit(Lexer *lexer)
{
	Token *token_array = lexer->token_array;
	for (int i = 0; i < array_length(token_array); i++) {
		if (token_array[i].literal != NULL)
			free(token_array[i].literal);
	}
	array_free(token_array);
}

void lexer_scan_tokens(Lexer *lexer)
{
	ASSERT(lexer->token_array != NULL, "lexer token array is invalid");
	ASSERT(lexer->buffer != NULL, "lexer buffer is invalid");

	Token token;
	do {
		token = scan_next_token(lexer);
		array_push(lexer->token_array, token);
	} while (token.type != TOKEN_EOF);
}

static Token scan_next_token(Lexer *lexer)
{
	// Skip indentation/spaces
	skip_whitespace(lexer);
	skip_comments(lexer);

	if (is_at_end(lexer)) {
		return (Token){TOKEN_EOF, NULL};
	}

	unsigned int start_position = lexer->next_pos;
	char c = consume(lexer);

	// Single characters
	switch(c) {
		case('~'): return (Token){TOKEN_TILDE, NULL};
		case('-'):
		   if (peek(lexer) != '-') {
			   return (Token){TOKEN_MINUS, NULL};
		   } else {
			   advance(lexer);
			   return (Token){TOKEN_DECREMENT, NULL};
			}
		   break;
		case('+'): return (Token){TOKEN_PLUS, NULL};
		case('*'): return (Token){TOKEN_ASTERISK, NULL};
		case('/'): return (Token){TOKEN_FORWARD_SLASH, NULL};
		case('%'): return (Token){TOKEN_PERCENT, NULL};
		case('&'): return (Token){TOKEN_AMPERSAND, NULL};
		case('|'): return (Token){TOKEN_PIPE, NULL};
		case('^'): return (Token){TOKEN_CARET, NULL};

		case('<'):
			if (peek(lexer) == '<') {
				advance(lexer);
				return (Token){TOKEN_LEFT_SHIFT, NULL};
			}
			break;
		case('>'):
			if (peek(lexer) == '>') {
				advance(lexer);
				return (Token){TOKEN_RIGHT_SHIFT, NULL};
			}
			break;

		case('('): return (Token){TOKEN_LPAREN, NULL};
		case(')'): return (Token){TOKEN_RPAREN, NULL};
		case('{'): return (Token){TOKEN_LBRACE, NULL};
		case('}'): return (Token){TOKEN_RBRACE, NULL};
		case(';'): return (Token){TOKEN_SEMICOLON, NULL};
	}

	// Others
	if (isdigit(c)) {
		return handle_number(lexer, start_position);
	} else if ((isalpha(c)) || c == '_') {
		return handle_keyword_identifier(lexer, start_position);
	}

	printf("Lexer error: invalid character %c\n", c);
	lexer->had_error = true;
	return (Token){TOKEN_INVALID, NULL};
}

static void skip_whitespace(Lexer *lexer)
{
	while(isspace(lexer->buffer[lexer->next_pos])){
		lexer->next_pos++;
	}
}

static void skip_comments(Lexer *lexer)
{
	// TODO: refactor this
	if (lexer->buffer[lexer->next_pos] == '/' &&
		lexer->buffer[lexer->next_pos + 1] == '/') {
		lexer->next_pos += 2;
		while (lexer->buffer[lexer->next_pos++] != '\n');
		skip_whitespace(lexer);
		skip_comments(lexer);
	}
}

static bool is_at_end(const Lexer *lexer)
{
	return (peek(lexer) == '\0');
}

static char consume(Lexer *lexer)
{
	return lexer->buffer[lexer->next_pos++];
}

static char peek(const Lexer *lexer)
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

	if (isalnum(peek(lexer)) || peek(lexer) == '_') {
		ERROR(lexer, "expected number, got identifier\n");
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
