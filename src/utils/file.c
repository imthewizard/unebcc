#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/file.h"

File file_open(const char *path)
{
	File file;
	file.filename = path;

	file.fp = fopen(path, "r+");
	if (file.fp == NULL) {
		printf("File '%s' does not exist\n", path);
		exit(EXIT_FAILURE);
	}

	return file;
}

File file_create(const char *path)
{
	File file;
	file.filename = path;

	file.fp = fopen(path, "w+");
	if (file.fp == NULL) {
		puts("fopen error");
		exit(EXIT_FAILURE);
	}

	return file;
}

void file_close(File *file)
{
	fclose(file->fp);
}

void file_remove(File *file)
{
	remove(file->filename);
}

void file_to_buffer(File *file, char **buffer, unsigned int *buffer_len)
{
	FILE *fp = file->fp;

	fseek(fp, 0, SEEK_END);
	int file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	*buffer = malloc(sizeof(char) * (file_size + 1));
	fread(*buffer, sizeof(char), file_size, fp);
	(*buffer)[file_size] = '\0';

	*buffer_len = file_size;
}

char *filename_without_extension(const char *filename)
{
	char *copy = strdup(filename);
	char *tmp = copy;
	while (*tmp != '.') tmp++;
	*tmp = '\0';
	return copy;
}

char *filename_extension(const char *filename, const char *extension)
{
	const int filename_len = strlen(filename);
	const int extension_len = strlen(extension);

	char *with_extension = malloc(sizeof(char) * (filename_len + extension_len + 1));

	memcpy(with_extension, filename, filename_len);
	memcpy(with_extension + filename_len, extension, extension_len);
	with_extension[filename_len + extension_len] = '\0';

	return with_extension;
}
