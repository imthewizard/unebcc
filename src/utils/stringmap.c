#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "utils/stringmap.h"
#include "utils/debug.h"

// FNV-1a hash. Key must be null-terminated
static uint32_t fnv1a_hash(const char *key);
// Inserts an entry into a list of entries, does not check for expansion
static void entry_put_unchecked(StringMapEntry *entries, int size, const char *key, void *value);
// Expands the map
static void expand_string_map(StringMap *sm);

StringMap string_map_init(void)
{
	StringMap sm;
	// Default size
	sm.entries = calloc(2, sizeof(StringMapEntry));
	sm.size = 2;
	sm.len = 0;
	return sm;
}

void string_map_deinit(StringMap *sm)
{
	for (int i = 0; i < sm->size; i++) {
		if (sm->entries[i].key != NULL) {
			free(sm->entries[i].key);
		}
	}
	free(sm->entries);
}

void string_map_put_unchecked(StringMap *sm, const char *key, void *value)
{
	ASSERT(key != NULL, "invalid key");
	ASSERT(value != NULL, "invalid value");
	entry_put_unchecked(sm->entries, sm->size, key, value);
	sm->len++;
}

void string_map_put(StringMap *sm, const char *key, void *value)
{
	ASSERT(key != NULL, "invalid key");
	ASSERT(value != NULL, "invalid value");

	const int size = sm->size;

	if (sm->len + 1 >= size) {
		expand_string_map(sm);
	}

	string_map_put_unchecked(sm, key, value);
}

void *string_map_get(const StringMap *sm, const char *key)
{
	int size = sm->size;
	StringMapEntry *entries = sm->entries;
	int index = fnv1a_hash(key) & (size - 1);
	int start_index = index;

	bool wrapped = false;
	while (sm->entries[index].key != NULL) {
		if (wrapped && (index == start_index)) {
			return NULL;
		}

		if (strcmp(entries[index].key, key) == 0) {
			return entries[index].value;
		}

		index++;
		if (index >= size) {
			index = 0;
			wrapped = true;
			continue;
		}
	}

	return NULL;
}

bool string_map_has(const StringMap *sm, const char *key)
{
	int size = sm->size;
	StringMapEntry *entries = sm->entries;
	int index = fnv1a_hash(key) & (size - 1);
	int start_index = index;

	bool wrapped = false;
	while (sm->entries[index].key != NULL) {
		if (wrapped && (index == start_index)) {
			return false;
		}

		if (strcmp(entries[index].key, key) == 0) {
			return true;
		}

		index++;
		if (index >= size) {
			index = 0;
			wrapped = true;
			continue;
		}
	}

	return false;
}

StringMapIterator string_map_iterator(StringMap *sm)
{
	return (StringMapIterator){.sm = sm, .entry = NULL, .next_index = 0};
}

bool string_map_next(StringMapIterator *i)
{
	ASSERT(i != NULL, "invalid iterator");

	const StringMap *sm = i->sm;
	while (i->next_index < sm->size) {
		StringMapEntry *current = &sm->entries[i->next_index++];
		if (current->key != NULL) {
			i->entry = current;
			return true;
		}
	}
	return false;
}

static uint32_t fnv1a_hash(const char *key)
{
	uint32_t hash = 2166136261;

	while (*key != '\0') {
		hash = hash ^ (*key++);
		hash = hash * 16777619;
	}

	return hash;
}

static void entry_put_unchecked(StringMapEntry *entries, int size, const char *key, void *value)
{
	int index = fnv1a_hash(key) & (size - 1);

	while (entries[index].key != NULL) {
		if (strcmp(entries[index].key, key) == 0) {
			entries[index].value = value;
			return;
		}

		index++;
		if (index >= size) {
			index = 0;
		}
	}

	char *key_copy = strdup(key);
	entries[index].key = key_copy;
	entries[index].value = value;
}

static void expand_string_map(StringMap *sm)
{
	int new_size = sm->size * 2;

	StringMapEntry *new_entries = calloc(new_size, sizeof(StringMapEntry));
	for (int i = 0; i < sm->size; i++) {
		const StringMapEntry *original_entry = &sm->entries[i];
		if (original_entry->key == NULL) continue;
		entry_put_unchecked(new_entries, new_size, original_entry->key, original_entry->value);
		free(original_entry->key);
	}

	free(sm->entries);
	sm->entries = new_entries;
	sm->size = new_size;
}
