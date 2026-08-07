#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "lexer/token.h"

static const char* token_type_str[] = {
	[TOKEN_IDENTIFIER] = "IDENTIFIER",
	[TOKEN_INTEGER_LITERAL] = "INTEGER_LITERAL",

	[TOKEN_VOID] = "VOID",
	[TOKEN_INT] = "INT",

	[TOKEN_RETURN] = "RETURN",

	[TOKEN_TILDE] = "TILDE",
	[TOKEN_MINUS] = "MINUS",

	[TOKEN_DECREMENT] = "DECREMENT",

	[TOKEN_LPAREN] = "LPAREN",
	[TOKEN_RPAREN] = "RPAREN",
	[TOKEN_LBRACE] = "LBRACE",
	[TOKEN_RBRACE] = "RBRACE",
	[TOKEN_SEMICOLON] = "SEMICOLON",

	[TOKEN_EOF] = "EOF",
	[TOKEN_INVALID] = "INVALID",
};

void print_token(Token *token)
{
	printf("%s", token_type_str[token->type]);
	if (token->literal == NULL) {
		printf(" - (null)\n");
	} else {
		printf(" - literal: %s\n", token->literal);
	}
}

Token *create_token_array(void)
{
	return NULL;
}

void free_token_array(Token *arr)
{
	Token *tmp = arr;
	while (tmp->type != TOKEN_EOF) {
		if (tmp->literal != NULL) {
			free(tmp->literal);
		}
		tmp++;
	}
	free(arr);
}

void push_token_array(Token **arr, int arr_size, const Token *value)
{
	// Reallocs by size + 1 every push, not super optimized but it's simple
	Token *tmp = realloc(*arr, sizeof(Token) * (arr_size + 1));
	if (tmp == NULL) {
		puts("push_token_array: realloc fail");
		exit(EXIT_FAILURE);
	}

	*arr = tmp;
	(*arr)[arr_size] = *value;
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
	assert(num_keywords == num_types);

	for (int i = 0; i < num_keywords; i++){
		if ((strcmp(keyword, keywords[i])) == 0){
			return keywords_types[i];
		}
	}

	return TOKEN_INVALID;
}
