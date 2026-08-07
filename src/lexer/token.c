#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "lexer/token.h"

// Turns type into a string and prints it
static void print_token_type(TokenType type);

void print_token(Token *token)
{
	print_token_type(token->type);
	if (token->literal == NULL) {
		printf(" - (null)\n");
	} else {
		printf(" - literal: %s\n", token->literal);
	}
}

static void print_token_type(TokenType type)
{
	switch(type){
		case(TOKEN_IDENTIFIER): printf("IDENTIFIER"); return;
		case(TOKEN_INTEGER_LITERAL): printf("INTEGER_LITERAL"); return;

		case(TOKEN_VOID): printf("VOID"); return;
		case(TOKEN_INT): printf("INT"); return;

		case(TOKEN_RETURN): printf("RETURN"); return;

		case(TOKEN_TILDE): printf("TILDE"); return;
		case(TOKEN_MINUS): printf("MINUS"); return;

		case(TOKEN_DECREMENT): printf("DECREMENT"); return;

		case(TOKEN_LPAREN): printf("LPAREN"); return;
		case(TOKEN_RPAREN): printf("RPAREN"); return;
		case(TOKEN_LBRACE): printf("LBRACE"); return;
		case(TOKEN_RBRACE): printf("RBRACE"); return;
		case(TOKEN_SEMICOLON): printf("SEMICOLON"); return;

		case(TOKEN_EOF): printf("EOF"); return;
		case(TOKEN_INVALID): printf("INVALID"); return;
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
