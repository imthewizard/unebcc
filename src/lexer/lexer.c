#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "utils/array.h"
#include "utils/debug.h"

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
Lexer *lexer = NULL;


// Sets had_error and prints an error
#define ERROR(lexer, msg) \
	do { \
		printf("Lexer error: %s", msg); \
		lexer->had_error = true; \
	} while (0)

// Increments lexer->pos to skip whitespace, if needed
static void skip_whitespace(void);
// Increments lexer->pos to skip comments, if needed
static void skip_comments(void);
// Checks if there are more chars to be read
static bool is_at_end(void);
// Returns the next character in the buffer and advance
static char consume(void);
// Returns the next character in the buffer, does not advance
static char peek(void);
// Advances to the next char
static void advance(void);
// Scans the string buffer and return a single token
static Token scan_next_token(void);

static Token handle_number(unsigned int start_pos);
static Token handle_keyword_identifier(unsigned int start_pos);

void lexer_init(const char *buffer, unsigned int buffer_len)
{
	ASSERT(lexer == NULL, "lexer was already initialized");
	lexer = malloc(sizeof(Lexer));

	lexer->token_array = array_create(lexer->token_array, 16);
	lexer->buffer = buffer;
	lexer->len = buffer_len;
	lexer->next_pos = 0;
	lexer->had_error = false;
}

void lexer_deinit(void)
{
	if (lexer != NULL) {
		Token *token_array = lexer->token_array;
		for (int i = 0; i < array_length(token_array); i++) {
			if (token_array[i].literal != NULL)
				free(token_array[i].literal);
		}
		array_free(token_array);
		free(lexer);
	}
}

void lexer_scan_tokens(void)
{
	ASSERT(lexer != NULL, "lexer was not initialized");

	Token token;
	do {
		token = scan_next_token();
		array_push(lexer->token_array, token);
	} while (token.type != TOKEN_EOF);
}

Token* lexer_get_token_array(void)
{
	return lexer->token_array;
}

bool lexer_had_error(void)
{
	return lexer->had_error;
}

static Token scan_next_token(void)
{
	// Skip indentation/spaces
	skip_whitespace();
	skip_comments();

	if (is_at_end()) {
		return (Token){TOKEN_EOF, NULL};
	}

	unsigned int start_position = lexer->next_pos;
	char c = consume();

	// Single characters
	switch(c) {
		case('-'):
			if (peek() == '-') {
				advance();
				return (Token){TOKEN_DECREMENT, NULL};
			} else {
				return (Token){TOKEN_MINUS, NULL};
			}
		case('&'):
			if (peek() == '&') {
				advance();
				return (Token){TOKEN_LOGICAL_AND, NULL};
			} else {
				return (Token){TOKEN_AMPERSAND, NULL};
			}
		case('|'):
			if (peek() == '|') {
				advance();
				return (Token){TOKEN_LOGICAL_OR, NULL};
			} else {
				return (Token){TOKEN_PIPE, NULL};
			}
		case('='):
			if (peek() == '=') {
				advance();
				return (Token){TOKEN_LOGICAL_EQUAL, NULL};
			} else {
				return (Token){TOKEN_EQUAL, NULL};
			}
		case('<'):
			if (peek() == '<') {
				advance();
				return (Token){TOKEN_LEFT_SHIFT, NULL};
			}
			if (peek() == '=') {
				advance();
				return (Token){TOKEN_LESS_EQUAL, NULL};
			}
			return (Token){TOKEN_LESS_THAN, NULL};
		case('>'):
			if (peek() == '>') {
				advance();
				return (Token){TOKEN_RIGHT_SHIFT, NULL};
			}
			if (peek() == '=') {
				advance();
				return (Token){TOKEN_GREATER_EQUAL, NULL};
			}
			return (Token){TOKEN_GREATER_THAN, NULL};
		case('!'):
			if (peek() == '=') {
				advance();
				return (Token){TOKEN_NOT_EQUAL, NULL};
			}
			return (Token){TOKEN_EXCLAMATION, NULL};

		case('+'): return (Token){TOKEN_PLUS, NULL};
		case('*'): return (Token){TOKEN_ASTERISK, NULL};
		case('/'): return (Token){TOKEN_FORWARD_SLASH, NULL};
		case('%'): return (Token){TOKEN_PERCENT, NULL};
		case('~'): return (Token){TOKEN_TILDE, NULL};
		case('^'): return (Token){TOKEN_CARET, NULL};
		case('('): return (Token){TOKEN_LPAREN, NULL};
		case(')'): return (Token){TOKEN_RPAREN, NULL};
		case('{'): return (Token){TOKEN_LBRACE, NULL};
		case('}'): return (Token){TOKEN_RBRACE, NULL};
		case(';'): return (Token){TOKEN_SEMICOLON, NULL};
	}

	// Others
	if (isdigit(c)) {
		return handle_number(start_position);
	} else if ((isalpha(c)) || c == '_') {
		return handle_keyword_identifier(start_position);
	}

	printf("Lexer error: invalid character %c\n", c);
	lexer->had_error = true;
	return (Token){TOKEN_INVALID, NULL};
}

static void skip_whitespace(void)
{
	while(isspace(lexer->buffer[lexer->next_pos])){
		lexer->next_pos++;
	}
}

static void skip_comments(void)
{
	// TODO: refactor this
	if (lexer->buffer[lexer->next_pos] == '/' &&
		lexer->buffer[lexer->next_pos + 1] == '/') {
		lexer->next_pos += 2;
		while (lexer->buffer[lexer->next_pos++] != '\n');
		skip_whitespace();
		skip_comments();
	}
}

static bool is_at_end(void)
{
	return (peek() == '\0');
}

static char consume(void)
{
	return lexer->buffer[lexer->next_pos++];
}

static char peek(void)
{
	return lexer->buffer[lexer->next_pos];
}

static void advance(void)
{
	lexer->next_pos++;
}

static Token handle_number(unsigned int start_pos)
{
	// Handling integers constants
	while (isdigit(peek())){
		advance();
	}

	if (isalnum(peek()) || peek() == '_') {
		ERROR(lexer, "expected number, got identifier\n");
	}

	unsigned int len = lexer->next_pos - start_pos;
	char *buf = malloc(sizeof(char) * (len + 1));
	strncpy(buf, &(lexer->buffer[start_pos]), len);
	buf[len] = '\0';

	return (Token){TOKEN_INTEGER_LITERAL, buf};
}

static Token handle_keyword_identifier(unsigned int start_pos)
{
	while (isalnum(peek()) || peek() == '_') {
		advance();
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
