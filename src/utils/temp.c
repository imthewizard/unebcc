#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "utils/temp.h"
#include "utils/os.h"
#include "utils/debug.h"

#ifdef UNEBCC_WINDOWS
#include <io.h>
static bool win32_temp_create(TemporaryFile *tmp)
{
	char template[] = "unebcctmpXXXXXX";

	char *result = _mktemp(template);
	if (result == NULL) {
		puts("_mktemp error");
		return false;
	}

	FILE *file = fopen(result, "w");
	if (file == NULL) {
		puts("fopen error");
		return false;
	}

	tmp->filename = result;
	tmp->file = file;
	return true;
}
#endif // UNEBCC_WINDOWS

#ifdef UNEBCC_LINUX
static bool linux_temp_create(TemporaryFile *tmp)
{
	char *template = malloc(sizeof(char) * 16);
	memcpy(template, "unebcctmpXXXXXX", 15);
	template[15] = '\0';

	int fd = mkstemp(template);
	if (fd == -1) {
		puts("mkstemp error");
		return false;
	}

	FILE *file = fdopen(fd, "w+");
	if (file == NULL) {
		puts("fdopen error");
		return false;
	}

	tmp->filename = template;
	tmp->file = file;
	return true;
}
#endif // UNEBCC_LINUX

bool temp_create(TemporaryFile *tmp)
{
#if defined(UNEBCC_WINDOWS)
	return win32_temp_create(tmp);
#elif defined(UNEBCC_LINUX)
	return linux_temp_create(tmp);
#else
	#error "Couldn't determine operating system: temp_create"
#endif
}

void temp_close(TemporaryFile *tmp)
{
	ASSERT(tmp->file != NULL, "closed tmp with null file");
	fclose(tmp->file);
	tmp->file = NULL;
}

void temp_free(TemporaryFile *tmp)
{
	ASSERT(tmp->filename != NULL, "tmp filename is null");
	if (tmp->file != NULL) {
		fclose(tmp->file);
	}
	remove(tmp->filename);
	free(tmp->filename);
}
