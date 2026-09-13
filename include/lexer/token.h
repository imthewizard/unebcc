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

	// Operators
	TOKEN_TILDE,         // ~
	TOKEN_MINUS,         // -
	TOKEN_PLUS,          // +
	TOKEN_ASTERISK,      // *
	TOKEN_FORWARD_SLASH, // /
	TOKEN_PERCENT,       // %
	TOKEN_AMPERSAND,     // &
	TOKEN_PIPE,          // |
	TOKEN_CARET,         // ^
	TOKEN_EQUAL,         // =
	TOKEN_EXCLAMATION,   // !
	TOKEN_LESS_THAN,     // <
	TOKEN_GREATER_THAN,  // >

	TOKEN_DECREMENT,     // --
	TOKEN_LEFT_SHIFT,    // <<
	TOKEN_RIGHT_SHIFT,   // >>
	TOKEN_LOGICAL_AND,   // &&
	TOKEN_LOGICAL_OR,    // ||
	TOKEN_LOGICAL_EQUAL, // ==
	TOKEN_NOT_EQUAL,     // !=
	TOKEN_LESS_EQUAL,    // <=
	TOKEN_GREATER_EQUAL, // >=

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
void print_token(const Token *token);
// Returns the token type as a string
const char *str_token_type(const TokenType type);
// Converts a null-terminated string keyword to a token type. Returns TOKEN_INVALID if no token for that keyword exists
TokenType keyword_to_tokentype(const char *keyword);

#endif // UNEBCC_TOKEN_H
