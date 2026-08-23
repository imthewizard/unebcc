#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backend/x86_64/function.h"
#include "backend/x86_64/x86_64.h"
#include "utils/array.h"

x86_64Function x86_64_function_init(const char *name)
{
	x86_64Function fn;
	fn.instructions = array_create(fn.instructions, 1);

	int name_len = strlen(name);
	fn.name = malloc(sizeof(char) * name_len + 1);
	strcpy(fn.name, name);

	return fn;
}

void x86_64_function_deinit(x86_64Function *fn)
{
	if (fn->name != NULL) {
		free(fn->name);
	}
	if (fn->instructions != NULL) {
		array_free(fn->instructions);
	}
}
