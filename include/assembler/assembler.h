#ifndef UNEBCC_ASSEMBLER_ASSEMBLER_H
#define UNEBCC_ASSEMBLER_ASSEMBLER_H

// Uses the installed gcc to assemble a file, returns error code (EXIT_SUCCESS if no error)
int assembler_assemble_gcc(const char *filename, const char *output);

#endif // UNEBCC_ASSEMBLER_ASSEMBLER_H
