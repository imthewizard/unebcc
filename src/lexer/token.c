#include <stdio.h>
#include <string.h>

#include "lexer/token.h"
#include "utils/debug.h"

static const char* token_type_str[] = {
	[TOKEN_IDENTIFIER] = "IDENTIFIER",
	[TOKEN_INTEGER_LITERAL] = "INTEGER_LITERAL",

	[TOKEN_VOID] = "VOID",
	[TOKEN_INT] = "INT",

	[TOKEN_RETURN] = "RETURN",

	[TOKEN_TILDE] = "TILDE",
	[TOKEN_MINUS] = "MINUS",
	[TOKEN_PLUS] = "PLUS",
	[TOKEN_ASTERISK] = "ASTERISK",
	[TOKEN_FORWARD_SLASH] = "FORWARD_SLASH",
	[TOKEN_PERCENT] = "PERCENT",
	[TOKEN_AMPERSAND] = "BITWISE_AND",
	[TOKEN_PIPE] = "BITWISE_OR",
	[TOKEN_CARET] = "BITWISE_XOR",
	[TOKEN_EQUAL] = "EQUAL",
	[TOKEN_EXCLAMATION] = "EXCLAMATION",
	[TOKEN_LESS_THAN] = "LESS_THAN",
	[TOKEN_GREATER_THAN] = "GREATER_THAN",

	[TOKEN_DECREMENT] = "DECREMENT",
	[TOKEN_LEFT_SHIFT] = "LEFT_SHIFT",
	[TOKEN_RIGHT_SHIFT] = "RIGHT_SHIFT",
	[TOKEN_LOGICAL_AND] = "LOGICAL_AND",
	[TOKEN_LOGICAL_OR] = "LOGICAL_OR",
	[TOKEN_LOGICAL_EQUAL] = "LOGICAL_EQUAL",
	[TOKEN_NOT_EQUAL] = "NOT_EQUAL",
	[TOKEN_LESS_EQUAL] = "LESS_EQUAL",
	[TOKEN_GREATER_EQUAL] = "GREATER_EQUAL",

	[TOKEN_LPAREN] = "LPAREN",
	[TOKEN_RPAREN] = "RPAREN",
	[TOKEN_LBRACE] = "LBRACE",
	[TOKEN_RBRACE] = "RBRACE",
	[TOKEN_SEMICOLON] = "SEMICOLON",

	[TOKEN_EOF] = "EOF",
	[TOKEN_INVALID] = "INVALID",
};

void print_token(const Token *token)
{
	printf("%s", token_type_str[token->type]);
	if (token->literal == NULL) {
		printf(" - (null)\n");
	} else {
		printf(" - literal: %s\n", token->literal);
	}
}

const char *str_token_type(const TokenType type)
{
	return token_type_str[type];
}

TokenType keyword_to_tokentype(const char *keyword)
{
	static const char *keywords[] = {
		"int", "void",
		"return",
	};
	static const TokenType keywords_types[] = {
		TOKEN_INT, TOKEN_VOID,
		TOKEN_RETURN,
	};

	const int num_keywords = sizeof(keywords) / sizeof(keywords[0]);
	const int num_types = sizeof(keywords_types) / sizeof(keywords_types[0]);
	ASSERT(num_keywords == num_types, "missing keyword in keywords or type in keywords_types");

	for (int i = 0; i < num_keywords; i++){
		if ((strcmp(keyword, keywords[i])) == 0){
			return keywords_types[i];
		}
	}

	return TOKEN_INVALID;
}
