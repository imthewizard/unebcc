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
#include "utils/temp.h"

// Returns an array of tokens.
static Token *lex(const char *buffer, unsigned int buffer_len, bool print);
// Parses an array of tokens and creates an AST
static Parser parse(const Token *token_array, bool print);
// Generates an IR from the AST
static IR generate_ir(const Parser *parser, bool print);
// Generates a x86_64 based IR from the general IR
static x86_64Program generate_machine_ir(const IR *ir, bool print);
// Generates assembly code from the machine IR and sends it to a temporary file
static TemporaryFile emit_assembly(const x86_64Program *prog, bool print);

void driver_start(const ArgsContext *ctx, const char *buffer, unsigned int buffer_len)
{
	Token *token_array = lex(buffer, buffer_len, ctx->print_lexer);
	Parser parser = parse(token_array, ctx->print_parser);
	IR ir = generate_ir(&parser, ctx->print_ir);
	x86_64Program prog = generate_machine_ir(&ir, ctx->print_machine_ir);

	// This function also closes the file, otherwise gcc won't be able to read it correctly
	TemporaryFile temp = emit_assembly(&prog, ctx->print_asm);

	assembler_assemble_gcc(temp.filename);

	temp_free(&temp);

	x86_64_program_deinit(&prog);
	ir_deinit(&ir);
	parser_deinit(&parser);
	for (int i = 0; i < array_length(token_array); i++) {
		if (token_array[i].literal != NULL)
			free(token_array[i].literal);
	}
	array_free(token_array);
}

static Token *lex(const char *buffer, unsigned int buffer_len, bool print)
{
	Token *token_arr = array_create(token_arr, 1);
	Lexer lexer;
	lexer_init(&lexer, buffer, buffer_len);
	lexer_scan_tokens(&lexer, &token_arr);
	if (print) {
		for (int i = 0; i < array_length(token_arr); i++) {
			print_token(&token_arr[i]);
		}
	}
	return token_arr;
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

static TemporaryFile emit_assembly(const x86_64Program *prog, bool print)
{
	TemporaryFile tmp;
	if (temp_create(&tmp) == false) {
		puts("Error creating temporary file");
		exit(EXIT_FAILURE);
	}
	x86_64_emit(tmp.file, prog);
	temp_close(&tmp);
	if (print) {
		x86_64_emit(stdout, prog);
	}
	return tmp;
}
