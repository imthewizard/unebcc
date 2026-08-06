#ifndef UNEBCC_TOKEN_H
#define UNEBCC_TOKEN_H

typedef enum TokenType{
	// Literals
	TOKEN_IDENTIFIER,
	TOKEN_INTEGER_LITERAL,

	// Keywords
	TOKEN_VOID,
	TOKEN_INT,

	TOKEN_RETURN,

	// Misc
	TOKEN_LPAREN,    // (
	TOKEN_RPAREN,    // )
	TOKEN_LBRACE,    // {
	TOKEN_RBRACE,    // }
	TOKEN_SEMICOLON, // ;

	TOKEN_EOF,
	TOKEN_INVALID,

}TokenType;

typedef struct Token{
	TokenType type;
	char *literal;
}Token;

// Prints the token's type and its literal value
void print_token(Token *token);

// Creates an array of tokens. Use push_token_array to insert values and free_token_array to deallocate
Token *create_token_array(void);
// Deallocates an array of tokens
void free_token_array(Token *arr);
// Reallocs an array of tokens and inserts a token
void push_token_array(Token **arr, int arr_size, const Token *value);

// Converts a null-terminated string keyword to a token type. Returns TOKEN_INVALID if no token for that keyword exists
TokenType keyword_to_tokentype(const char *keyword);

#endif // UNEBCC_TOKEN_H
