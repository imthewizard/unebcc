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
#include "preprocessor/preprocessor.h"
#include "semantic/semantic.h"

#include "args.h"
#include "utils/array.h"
#include "utils/file.h"

// Runs the preprocessing stage on the input and returns the preprocessed file
static File preprocess(const ArgsContext *ctx);
// Main compile pass: lexer, parser, semantic analysis and irs
static void compile(const ArgsContext *ctx, File *input, x86_64Program *program);
// Assembles the program
static void assemble(const ArgsContext *ctx, x86_64Program *program);

// Lexes a buffer
static void lex(const char *buffer, unsigned int buffer_len, bool print);
// Parses the lexer's array of tokens and creates an AST
static void parse(const Token *tokens, bool print);
// Runs a semantic analysis on the parser's AST and directly modifies it. Returns true if no errors occured
static bool semantic(ASTNode *ast, bool print);
// Generates an IR from the AST
static void generate_ir(const ASTNode *ast, bool print);
// Generates a x86_64 based IR from the general IR
static x86_64Program generate_machine_ir(const IRInstruction *insts, bool print);
// Generates assembly code from the machine IR and sends it to a file
static File emit_assembly(const char *filename, const x86_64Program *prog, bool print);

void driver_start(const ArgsContext *ctx)
{
	File preprocessed = preprocess(ctx);

	x86_64Program program;
	compile(ctx, &preprocessed, &program);

	assemble(ctx, &program);

	x86_64_program_deinit(&program);
	ir_deinit();
}

static File preprocess(const ArgsContext *ctx)
{
	char *filename_no_ext = filename_without_extension(ctx->filename);
	char *preprocessed_name = filename_extension(filename_no_ext, ".i");
	preprocessor_preprocess_gcc(ctx->filename, preprocessed_name);

	File temp = file_open(preprocessed_name);

	free(preprocessed_name);
	free(filename_no_ext);
	return temp;
}

static void compile(const ArgsContext *ctx, File *input, x86_64Program *program)
{
	const bool print_lexer      = args_cmp_value(ctx, "print", "lexer");
	const bool print_parser     = args_cmp_value(ctx, "print", "parser");
	const bool print_semantic   = args_cmp_value(ctx, "print", "semantic");
	const bool print_ir         = args_cmp_value(ctx, "print", "ir");
	const bool print_machine_ir = args_cmp_value(ctx, "print", "machine-ir");

	const bool only_lex      = args_has(ctx, "lex");
	const bool only_parse    = args_has(ctx, "parse");
	const bool only_semantic = args_has(ctx, "validate");
	const bool only_codegen  = args_has(ctx, "codegen");

	char *file_buffer;
	unsigned int buffer_len;
	file_to_buffer(input, &file_buffer, &buffer_len);

	bool can_exit = false;
	bool had_error = false;

	lex(file_buffer, buffer_len, print_lexer);
	if (lexer_had_error()) had_error = true;
	free(file_buffer);
	if (only_lex) {
		can_exit = true;
		goto cleanup;
	}

	parse(lexer_get_token_array(), print_parser);
	if (parser_had_error()) had_error = true;
	if (only_parse) {
		can_exit = true;
		goto cleanup;
	}

	if (!semantic(parser_get_ast(), print_semantic)) {
		can_exit = true;
		had_error = true;
	}
	if (only_semantic || had_error) {
		can_exit = true;
		goto cleanup;
	}

	generate_ir(parser_get_ast(), print_ir);
	*program = generate_machine_ir(ir_get_instructions(), print_machine_ir);

	if (only_codegen)
		can_exit = true;

cleanup:
	file_close(input);
	file_remove(input);
	parser_deinit();
	lexer_deinit();
	semantic_free_allocated();

	if (can_exit)
		exit(had_error ? EXIT_FAILURE : EXIT_SUCCESS);
}

static void assemble(const ArgsContext *ctx, x86_64Program *program)
{
	const bool print_asm = args_cmp_value(ctx, "print", "asm");

	char *filename_no_ext = filename_without_extension(ctx->filename);
	char *asm_filename = filename_extension(filename_no_ext, ".s");
	File temp = emit_assembly(asm_filename, program, print_asm);
	// Close the file now so gcc can read it correctly
	file_close(&temp);

	int ret = assembler_assemble_gcc(asm_filename, filename_no_ext);

	file_remove(&temp);

	free(asm_filename);
	free(filename_no_ext);

	if (ret != EXIT_SUCCESS) {
		x86_64_program_deinit(program);
		puts("Assembler error");
		exit(EXIT_FAILURE);
	}
}

static void lex(const char *buffer, unsigned int buffer_len, bool print)
{
	lexer_init(buffer, buffer_len);
	lexer_scan_tokens();
	if (print) {
		const Token *token_arr = lexer_get_token_array();
		for (int i = 0; i < array_length(token_arr); i++) {
			print_token(&token_arr[i]);
		}
	}
}

static void parse(const Token *tokens, bool print)
{
	parser_init(tokens);
	parser_parse();
	if (print) {
		ast_print(parser_get_ast(), 0);
	}
}

static bool semantic(ASTNode *ast, bool print)
{
	bool ok = semantic_analysis(ast);
	if (ok && print) {
		ast_print(ast, 0);
	}
	return ok;
}

static void generate_ir(const ASTNode *ast, bool print)
{
	ir_init();
	ir_generate(ast);
	if (print) {
		ir_print();
	}
}

static x86_64Program generate_machine_ir(const IRInstruction *insts, bool print)
{
	x86_64Program prog = x86_64_program_init();
	x86_64_create_prog(insts, &prog);
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
