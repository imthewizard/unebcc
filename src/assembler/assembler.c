#include <stdlib.h>
#include <string.h>

#include "assembler/assembler.h"

static void call_gcc(char const *filename, const char *output)
{
	const char *main = "gcc -x assembler ";
	const int main_len = strlen(main);
	const int filename_len = strlen(filename);
	const int flag_o_len = strlen(" -o ");
	const int output_len = strlen(output);

	const int cmd_len = main_len + filename_len + flag_o_len + output_len;
	char *cmd = malloc(sizeof(char) * cmd_len);

	memcpy(cmd, main, main_len);
	memcpy(cmd + main_len, filename, filename_len);
	memcpy(cmd + main_len + filename_len, " -o ", flag_o_len);
	memcpy(cmd + main_len + filename_len + flag_o_len, output, output_len);

	system(cmd);
	free(cmd);
}

void assembler_assemble_gcc(const char *filename, const char *output)
{
	call_gcc(filename, output);
}
