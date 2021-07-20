#include <stdlib.h>
#include <stdio.h>
#include <ghh/hashmap.h>
#include <ghh/string.h>
#include <ghh/array.h>

hashmap_t *mem_active = NULL;
array_t *mem_inactive = NULL;
array_t *mem_unmatched = NULL;

typedef struct memlog_entry {
    string_t *file;
    int line;
} memlog_entry_t;

void memcheck_init() {
    mem_active = hashmap_create(0, sizeof(void *), true);
    mem_inactive = array_create(0);
    mem_unmatched = array_create(0);
}

void memcheck_quit() {
    memlog_entry_t *entry;
    size_t i;

    for (i = 0; i < array_size(mem_unmatched); ++i) {
        entry = array_get(mem_unmatched, i);
        printf(
            "MEMCHECK: unmatched free at %s:%d\n",
            string_raw(entry->file), entry->line
        );
    }

    if (hashmap_size(mem_active)) {
        HMAP_FOREACH_V(entry, mem_active)
            printf(
                "MEMCHECK: unfreed allocation at %s:%d\n",
                string_raw(entry->file), entry->line
            );
    } else {
        printf("MEMCHECK: all matched allocations freed\n");
    }

    HMAP_FOREACH_V(entry, mem_active)
        string_destroy(entry->file);

    for (i = 0; i < array_size(mem_inactive); ++i) {
        entry = array_get(mem_inactive, i);
        string_destroy(entry->file);
    }

    for (i = 0; i < array_size(mem_unmatched); ++i) {
        entry = array_get(mem_unmatched, i);
        string_destroy(entry->file);
    }

    hashmap_destroy(mem_active, true);
    array_destroy(mem_inactive, true);
    array_destroy(mem_unmatched, true);
}

void *ghh_alloc(size_t size, const char *file, const int line) {
    void *ptr = calloc(1, size);
    memlog_entry_t *entry = malloc(sizeof(*entry));

    entry->file = string_create(file);
    entry->line = line;

    hashmap_set(mem_active, &ptr, entry);

    return ptr;
}

void ghh_free(void *ptr, const char *file, const int line) {
    memlog_entry_t *entry;

    if ((entry = hashmap_remove(mem_active, &ptr)) != NULL) {
        array_push(mem_inactive, entry);
    } else {
        entry = malloc(sizeof(*entry));

        entry->file = string_create(file);
        entry->line = line;

        array_push(mem_unmatched, entry);
    }

    free(ptr);
}

void *ghh_realloc(void *old_ptr, size_t size, const char *file, const int line) {
    if (old_ptr == NULL)
        return ghh_alloc(size, file, line);
    else if (size == 0)
        return (ghh_free(old_ptr, "ghh_realloc", 0), NULL);

    return realloc(old_ptr, size);
}
