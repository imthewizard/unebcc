#ifndef UNEBCC_UTILS_STRINGMAP_H
#define UNEBCC_UTILS_STRINGMAP_H

#include <stdbool.h>

typedef struct StringMapEntry {
	char *key;
	void *value;
} StringMapEntry;

typedef struct StringMap {
	StringMapEntry *entries;
	int size;
	int len;
} StringMap;

typedef struct StringMapIterator {
	StringMap *sm;
	StringMapEntry *entry;
	int next_index;
} StringMapIterator;

// Creates a string map
StringMap string_map_init(void);
// Frees a string map
void string_map_deinit(StringMap *sm);
// Inserts a key and a value into the string map. If the key already exists, updates the value
// Does not check if the map needs to expand before insertion
void string_map_put_unchecked(StringMap *sm, const char *key, void *value);
// Same as the unchecked variant, but expands the map if needed.
void string_map_put(StringMap *sm, const char *key, void *value);
// Returns the value of the key or null if it does not exist
void *string_map_get(const StringMap *sm, const char *key);
// Returns true if the key exists in the map
bool string_map_has(const StringMap *sm, const char *key);

// Creates a iterator for sm
StringMapIterator string_map_iterator(StringMap *sm);
// Sets entry to the next valid entry. Returns false if we reached the end of the map
bool string_map_next(StringMapIterator *i);

#endif // UNEBCC_UTILS_STRINGMAP_H
