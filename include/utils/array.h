#ifndef UNEBCC_DS_ARRAY_H
#define UNEBCC_DS_ARRAY_H

#include <stdlib.h> // free
#include <string.h> // memmove

#define ARRAY_GROW_MULTIPLIER 1.5

typedef struct _ArrayHeader {
	int elements;
	int max_elements;
} _ArrayHeader;

void *_array_init(int element_size, int array_size);
void *_array_grow(void *arr, int element_size);

#define _ARRAY_HEADER(arr) ((_ArrayHeader*) ((char*)arr - sizeof(_ArrayHeader)))
#define _ARRAY_NEED_GROW(arr) ( _ARRAY_HEADER(arr)->elements + 1 > _ARRAY_HEADER(arr)->max_elements )

#define array_create(arr, size) (_array_init(sizeof(*arr), size))
#define array_length(arr) ( _ARRAY_HEADER(arr)->elements )
#define array_max_length(arr) ( _ARRAY_HEADER(arr)->max_elements )
#define array_free(arr) ( free(_ARRAY_HEADER(arr)) )

#define array_push(arr, element) \
	do { \
		if (_ARRAY_NEED_GROW(arr)) { \
			(arr) = _array_grow((arr), sizeof(*(arr))); \
		} \
		(arr)[_ARRAY_HEADER(arr)->elements++] = (element); \
	} while (0)

#define array_insert(arr, element, pos) \
	do { \
		if (_ARRAY_NEED_GROW(arr)) { \
			(arr) = _array_grow((arr), sizeof(*(arr))); \
		} \
		int len = array_length(arr); \
		memmove((arr) + (pos) + 1, (arr) + (pos), sizeof(*(arr)) * (len - (pos))); \
		(arr)[(pos)] = (element); \
		_ARRAY_HEADER(arr)->elements++; \
	} while(0)

#endif // UNEBCC_DS_ARRAY_H
