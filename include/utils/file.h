#ifndef UNEBCC_UTILS_FILE_H
#define UNEBCC_UTILS_FILE_H

#include <stdio.h>

typedef struct File {
	const char *filename;
	FILE *fp;
} File;

// Opens a file for reading and writing
File file_open(const char *path);
// Creates a file for reading and writing
File file_create(const char *path);
// Closes a file
void file_close(File *file);
// Removes a file
void file_remove(File *file);
// Reads a file contents and size.
void file_to_buffer(const File *file, char **buffer, unsigned int *buffer_len);

// Returns the same filename but without the extension. Must be freed later
char *filename_without_extension(const char *filename);
// Returns the same filename but with an extension. Must be freed later
char *filename_extension(const char *filename, const char *extension);

#endif // UNEBCC_UTILS_FILE_H
