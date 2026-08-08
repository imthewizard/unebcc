#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ir/function.h"
#include "ir/basic_block.h"

void function_print(IRFunction *fn)
{
	for (int i = 0; i < fn->basic_block_amount; i++) {
		basic_block_print(&fn->basic_blocks[i]);
	}
}

void function_init(IRFunction *fn)
{
	fn->basic_blocks = NULL;
	fn->basic_block_amount = 0;
}

void function_deinit(IRFunction *fn)
{
	if (fn->basic_blocks == NULL) return;

	for (int i = 0; i < fn->basic_block_amount; i++) {
		basic_block_deinit(&fn->basic_blocks[i]);
	}

	free(fn->basic_blocks);
}

void function_push(IRFunction *fn, const IRBasicBlock *bb)
{
	IRBasicBlock *tmp = realloc(fn->basic_blocks, sizeof(IRBasicBlock) * (fn->basic_block_amount + 1));
	if (tmp == NULL) {
		puts("function_push: realloc fail");
		exit(EXIT_FAILURE);
	}

	fn->basic_blocks = tmp;
	fn->basic_blocks[fn->basic_block_amount++] = *bb;
}
