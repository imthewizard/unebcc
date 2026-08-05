#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "token.h"

void print_usage(void);
void read_file(const char *filename, char **buffer, unsigned int *buffer_len);

int main(int argc, char **argv)
{
	if (argc < 2){
		print_usage();
		return EXIT_FAILURE;
	}

	const char *filename = argv[1];

	char *file_buffer = NULL;
	unsigned int file_len;
	read_file(filename, &file_buffer, &file_len);

	Token *token_arr = create_token_array();
	Lexer lexer;
	lexer_init(&lexer, file_buffer, file_len);
	lexer_scan_tokens(&lexer, &token_arr);

	puts("Tokens: ");
	Token *tmp = token_arr;
	while (tmp->type != TOKEN_EOF) {
		print_token(tmp++);
	}
	print_token(tmp);

	free_token_array(token_arr);
	free(file_buffer);
	return EXIT_SUCCESS;
}

void print_usage(void)
{
	puts("TODO: usage");
}

void read_file(const char *filename, char **buffer, unsigned int *buffer_len)
{
	FILE *file = fopen(filename, "r");
	if (file == NULL){
		return;
	}

	fseek(file, 0, SEEK_END);
	int file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	*buffer = malloc(sizeof(char) * (file_size + 1));
	fread(*buffer, sizeof(char), file_size, file);
	(*buffer)[file_size] = '\0';

	*buffer_len = file_size;
	fclose(file);
}
