#include <stdio.h>
#include <stdlib.h>

#include "utils/array.h"
#include "utils/debug.h"

void *_array_init(int element_size, int array_size)
{
	ASSERT(array_size > 0, "invalid array size");

	void *arr = malloc(sizeof(_ArrayHeader) + (element_size * array_size));

	_ArrayHeader *header = (_ArrayHeader*)arr;
	header->elements = 0;
	header->max_elements = array_size;

	return (char*)arr + sizeof(_ArrayHeader);
}

void *_array_grow(void *arr, int element_size)
{
	_ArrayHeader *header = _ARRAY_HEADER(arr);
	int max_elements = header->max_elements;

	int next_size = (int)(max_elements * ARRAY_GROW_MULTIPLIER) + 1;

	void *tmp = realloc((void*)header, sizeof(_ArrayHeader) + (next_size * element_size));
	if (tmp == NULL) {
		puts("_array_grow fail: out of memory?");
		exit(EXIT_FAILURE);
	}

	_ArrayHeader *new_header = (_ArrayHeader*)tmp;
	new_header->max_elements = next_size;

	return (char*)tmp + sizeof(_ArrayHeader);
}
