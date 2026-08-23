#include <assert.h>

#include "ir/function.h"
#include "ir/basic_block.h"
#include "utils/array.h"

void function_print(IRFunction *fn)
{
	for (int i = 0; i < array_length(fn->basic_blocks); i++) {
		basic_block_print(&fn->basic_blocks[i]);
	}
}

void function_init(IRFunction *fn)
{
	fn->basic_blocks = array_create(fn->basic_blocks, 1);
}

void function_deinit(IRFunction *fn)
{
	if (fn->basic_blocks == NULL) return;
	array_free(fn->basic_blocks);
}
