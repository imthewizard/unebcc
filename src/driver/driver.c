#include <stdlib.h>
#include <stdbool.h>

#include "driver/driver.h"

#include "backend/x86_64/program.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ir/ir.h"
#include "backend/x86_64/gen.h"
#include "backend/x86_64/emitter.h"
#include "assembler/assembler.h"

#include "args.h"
#include "utils/array.h"
#include "utils/file.h"

// Lexes a buffer and returns the lexer an array of tokens
static Lexer lex(const char *buffer, unsigned int buffer_len, bool print);
// Parses an array of tokens and creates an AST
static Parser parse(const Token *token_array, bool print);
// Generates an IR from the AST
static IR generate_ir(const Parser *parser, bool print);
// Generates a x86_64 based IR from the general IR
static x86_64Program generate_machine_ir(const IR *ir, bool print);
// Generates assembly code from the machine IR and sends it to a file
static File emit_assembly(const char *filename, const x86_64Program *prog, bool print);

void driver_start(const ArgsContext *ctx)
{
	const bool print_lexer      = args_cmp_value(ctx, "print", "lexer");
	const bool print_parser     = args_cmp_value(ctx, "print", "parser");
	const bool print_ir         = args_cmp_value(ctx, "print", "ir");
	const bool print_machine_ir = args_cmp_value(ctx, "print", "machine-ir");
	const bool print_asm        = args_cmp_value(ctx, "print", "asm");

	const bool only_lex     = args_has(ctx, "lex");
	const bool only_parse   = args_has(ctx, "parse");
	const bool only_codegen = args_has(ctx, "codegen");

	File file = file_open(ctx->filename);
	char *file_buffer;
	unsigned int buffer_len;
	file_to_buffer(&file, &file_buffer, &buffer_len);

	Lexer lexer = lex(file_buffer, buffer_len, print_lexer);
	if (only_lex) {
		if (lexer.had_error) exit(EXIT_FAILURE);
		exit(EXIT_SUCCESS);
	}

	free(file_buffer);

	Parser parser = parse(lexer.token_array, print_parser);
	if (parser.had_error) exit(EXIT_FAILURE);
	if (only_parse) exit(EXIT_SUCCESS);


	IR ir = generate_ir(&parser, print_ir);
	x86_64Program prog = generate_machine_ir(&ir, print_machine_ir);
	if (only_codegen) exit(EXIT_SUCCESS);

	char *filename_no_ext = filename_without_extension(ctx->filename);
	char *asm_filename = filename_extension(filename_no_ext, ".s");
	File temp = emit_assembly(asm_filename, &prog, print_asm);
	// Close the file now so gcc can read it correctly
	file_close(&temp);

	assembler_assemble_gcc(asm_filename, filename_no_ext);

	file_remove(&temp);

	free(asm_filename);
	free(filename_no_ext);

	x86_64_program_deinit(&prog);
	ir_deinit(&ir);
	parser_deinit(&parser);
	lexer_deinit(&lexer);
}

static Lexer lex(const char *buffer, unsigned int buffer_len, bool print)
{
	Lexer lexer;
	lexer_init(&lexer, buffer, buffer_len);
	lexer_scan_tokens(&lexer);
	if (print) {
		const Token *token_arr = lexer.token_array;
		for (int i = 0; i < array_length(token_arr); i++) {
			print_token(&token_arr[i]);
		}
	}
	return lexer;
}

static Parser parse(const Token *token_array, bool print)
{
	Parser parser;
	parser_init(&parser, token_array);
	parser_parse(&parser);
	if (print) {
		ast_print(parser.ast, 0);
	}
	return parser;
}

static IR generate_ir(const Parser *parser, bool print)
{
	IR ir;
	ir_init(&ir);
	ir_generate(&ir, parser->ast);
	if (print) {
		ir_print(&ir);
	}
	return ir;
}

static x86_64Program generate_machine_ir(const IR *ir, bool print)
{
	x86_64Program prog = x86_64_program_init();
	x86_64_create_prog(ir, &prog);
	if (print) {
		x86_64_program_print(&prog);
	}
	return prog;
}

static File emit_assembly(const char *filename, const x86_64Program *prog, bool print)
{
	File file = file_create(filename);
	x86_64_emit(file.fp, prog);
	if (print) {
		x86_64_emit(stdout, prog);
	}
	return file;
}
