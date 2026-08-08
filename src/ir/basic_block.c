#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ir/basic_block.h"

void basic_block_print(IRBasicBlock *bb)
{
	for (int i = 0; i < bb->instruction_amount; i++) {
		instruction_print(&bb->instructions[i]);
	}
}

void basic_block_init(IRBasicBlock *bb)
{
	bb->instructions = NULL;
	bb->instruction_amount = 0;
}

void basic_block_deinit(IRBasicBlock *bb)
{
	if (bb->instructions == NULL) return;

	free(bb->instructions);
}

void basic_block_push(IRBasicBlock *bb, const IRInstruction *inst)
{
	IRInstruction *tmp = realloc(bb->instructions, sizeof(IRInstruction) * (bb->instruction_amount + 1));
	if (tmp == NULL) {
		puts("basic_block_push: realloc fail");
		exit(EXIT_FAILURE);
	}

	bb->instructions = tmp;
	bb->instructions[bb->instruction_amount++] = *inst;
}
