#ifndef UNEBCC_UTILS_TEMP_H
#define UNEBCC_UTILS_TEMP_H

#include <stdio.h>
#include <stdbool.h>

typedef struct TemporaryFile {
	char *filename;
	FILE *file;
} TemporaryFile;

// Attempts to create a temporary file. Returns false if it fails
bool temp_create(TemporaryFile *tmp);
// Closes a temporary file
void temp_close(TemporaryFile *tmp);
// Frees a temporary file and closes it if's still open, also deletes the file
void temp_free(TemporaryFile *tmp);


#endif // UNEBCC_UTILS_TEMP_H
