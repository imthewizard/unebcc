#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"

#include "driver/driver.h"

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
	} else if (ctx.ok == false) {
		return EXIT_FAILURE;
	}

	char *file_buffer = NULL;
	unsigned int file_len;
	read_file(ctx.filename, &file_buffer, &file_len);

	if (file_buffer == NULL) {
		printf("File \"%s\" does not exist", ctx.filename);
		return EXIT_FAILURE;
	}

	driver_start(&ctx, file_buffer, file_len);

	free(file_buffer);
	return EXIT_SUCCESS;
}

void print_usage(const char *cmd)
{
	printf("%s [FILE] [FLAGS]\n", cmd);
	puts("Flags:");
	puts("--lexer: prints the lexed tokens");
	puts("--parser: prints the AST");
	puts("--ir: prints the IR");
	puts("--machine-ir: prints the machine IR");
	puts("--print-asm: prints the assembly output");
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
