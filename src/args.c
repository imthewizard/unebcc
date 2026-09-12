#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "args.h"
#include "utils/array.h"
#include "utils/stringmap.h"

ArgsContext args_init(void)
{
	ArgsContext ctx;
	ctx.user_args = string_map_init();
	ctx.available_args = string_map_init();
	ctx.filename = NULL;
	ctx.ok = true;

	return ctx;
}

void args_deinit(ArgsContext *ctx)
{
	StringMapIterator it = string_map_iterator(&ctx->available_args);
	while (string_map_next(&it)) {
		free(it.entry->value);
	}
	string_map_deinit(&ctx->user_args);
	string_map_deinit(&ctx->available_args);
}

void args_add_arg(ArgsContext *ctx, const char *arg, const char *desc, bool provides_value)
{
	static int i = 0;

	ArgInfo *info = malloc(sizeof(ArgInfo));
	info->name = arg;
	info->description = desc;
	info->requires_value = provides_value;
	info->index = i++;

	string_map_put(&ctx->available_args, arg, (void*)info);
}

void args_print_help(ArgsContext *ctx)
{
	ArgInfo *ordered_info = array_create_zeroes(ordered_info, ctx->available_args.len);

	StringMapIterator it = string_map_iterator(&ctx->available_args);
	while (string_map_next(&it)) {
		const ArgInfo *info = it.entry->value;
		array_set(ordered_info, *info, info->index);
	}

	puts("Available flags:");
	for (int i = 0; i < array_length(ordered_info); i++) {
		const ArgInfo *info = &ordered_info[i];
		if (info->requires_value) {
			printf("--%s=[value]: %s\n", info->name, info->description);
		} else {
			printf("--%s: %s\n", info->name, info->description);
		}
	}

	array_free(ordered_info);
}

void args_parse(ArgsContext *ctx, int argc, char **argv, int start)
{
	for (int i = start; i < argc; i++) {
		char *full_arg = argv[i];
		char *arg = full_arg + 2; // skip --

		ArgInfo *info = string_map_get(&ctx->available_args, arg);
		if (info != NULL) {
			if (info->requires_value) {
				char *value = argv[++i];
				string_map_put(&ctx->user_args, arg, value);
			} else {
				// setting value to the same thing as key
				string_map_put(&ctx->user_args, arg, arg);
			}
			continue;
		}

		if (ctx->filename == NULL) {
			ctx->filename = full_arg;
		} else {
			printf("Invalid argument: %s\n", full_arg);
			ctx->ok = false;
		}
	}
}

bool args_has(const ArgsContext *ctx, const char *arg)
{
	return string_map_has(&ctx->user_args, arg);
}

bool args_cmp_value(const ArgsContext *ctx, const char *arg, const char *expected)
{
	const char *value = string_map_get(&ctx->user_args, arg);
	if (value == NULL) return false;

	return (strcmp(value, expected) == 0);
}
