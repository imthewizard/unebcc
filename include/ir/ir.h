#ifndef UNEBCC_IR_H
#define UNEBCC_IR_H

// #include "ir/basic_block.h"
// #include "ir/instruction.h"
#include "ir/function.h"
#include "parser/ast.h"

typedef struct IR {
	IRFunction *functions;
}IR;

void ir_print(IR *ir);

// Creates a new IR
void ir_init(IR *ir);
// Frees an IR
void ir_deinit(IR *ir);
// Generates the IR for the specified ast
void ir_generate(IR *ir, ASTNode *ast);

#endif // UNEBCC_IR_H
