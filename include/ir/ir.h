#ifndef UNEBCC_IR_IR_H
#define UNEBCC_IR_IR_H

#include "instruction.h"
#include "parser/ast.h"

void ir_print(void);

// Creates a new IR
void ir_init(void);
// Frees an IR
void ir_deinit(void);
// Generates the IR for the specified ast
void ir_generate(const ASTNode *ast);
// Gets the generated IR instructions (call after ir_generate)
IRInstruction *ir_get_instructions(void);

#endif // UNEBCC_IR_IR_H
