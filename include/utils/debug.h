#ifndef UNEBCC_UTILS_DEBUG_H
#define UNEBCC_UTILS_DEBUG_H

#ifndef NDEBUG
#include <stdio.h>
#include <stdlib.h>
#define ASSERT(expr, msg) do { \
		if (!(expr)) { \
			printf("Assertion fail: %s\nat: %s:%d\nin: %s\n", msg, __FILE__, __LINE__, __func__); \
			exit(EXIT_FAILURE); \
		} \
	} while (0)
#define UNIMPLEMENTED(msg) \
		printf("Reached unimplemented part: %s\nat: %s:%d\nin: %s\n", msg, __FILE__, __LINE__, __func__); \
		exit(EXIT_FAILURE)
#else
#define ASSERT(expr, msg)
#define UNIMPLEMENTED(msg)
#endif // NDEBUG

#endif // UNEBCC_UTILS_DEBUG_H
