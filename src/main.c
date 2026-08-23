#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ir/ir.h"
#include "backend/x86_64/gen.h"

#include "utils/array.h"

void print_usage(const char *cmd);
void read_file(const char *filename, char **buffer, unsigned int *buffer_len);

int main(int argc, char **argv)
{
	if (argc < 2){
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	ArgsContext ctx = args_parse(argc, argv, 1);

	if (ctx.filename == NULL) {
		puts("Missing file");
		return EXIT_FAILURE;
	}

	char *file_buffer = NULL;
	unsigned int file_len;
	read_file(ctx.filename, &file_buffer, &file_len);

	if (file_buffer == NULL) {
		printf("File \"%s\" does not exist", ctx.filename);
		return EXIT_FAILURE;
	}

	Token *token_arr = array_create(token_arr, 1);
	Lexer lexer;
	lexer_init(&lexer, file_buffer, file_len);
	lexer_scan_tokens(&lexer, &token_arr);

	if (ctx.print_tokens) {
		puts("\nTokens: ");
		for (int i = 0; i < array_length(token_arr); i++) {
			print_token(&token_arr[i]);
		}
	}

	Parser parser;
	parser_init(&parser, token_arr);
	parser_parse(&parser);

	if (ctx.print_ast) {
		puts("\nAST: ");
		ast_print(parser.ast, 0);
	}

	IR ir;
	ir_init(&ir);
	ir_generate(&ir, parser.ast);

	if (ctx.print_ir) {
		puts("\nIR: ");
		ir_print(&ir);
	}

	x86_64Program prog = x86_64_program_init();
	x86_64_create_prog(&ir, &prog);
	x86_64_program_print(&prog);


	x86_64_program_deinit(&prog);
	ir_deinit(&ir);
	parser_deinit(&parser);
	array_free(token_arr);
	free(file_buffer);
	return EXIT_SUCCESS;
}

void print_usage(const char *cmd)
{
	printf("%s [FILE] [FLAGS]\n", cmd);
	puts("Flags:");
	puts("--token: prints the lexed tokens");
	puts("--ast: prints the AST");
	puts("--ir: prints the IR");
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
