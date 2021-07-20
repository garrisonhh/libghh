#include <stdlib.h>
#include <stdio.h>
#include <ghh/hashmap.h>
#include <ghh/string.h>
#include <ghh/array.h>

// *** DO NOT INCLUDE <ghh/memcheck.h> ***

hashmap_t *mem_active = NULL;
hashmap_t *mem_inactive = NULL;

void memcheck_init() {
    mem_active = hashmap_create(0, -1, true);
    mem_inactive = hashmap_create(0, -1, true);
}

void memcheck_quit() {
    char *entry;
    size_t *size;

    HMAP_FOREACH_KV(entry, size, mem_active)
        printf("GHH_MEMCHECK: %lu bytes unfreed at %s.\n", *size, entry);

    HMAP_FOREACH_KV(entry, size, mem_inactive) {
        if (size == NULL)
            printf("GHH_MEMCHECK: unmatched free at %s.\n", entry);
        else
            printf("freed %lu at %s.\n", *size, entry);
    }

    hashmap_destroy(mem_active, true);
    hashmap_destroy(mem_inactive, true);
}

static inline void gen_entry(char *str, const char *file, const int line) {
    sprintf(str, "%s:%d", file, line);
}

void *ghh_alloc(size_t size, const char *file, const int line) {
    char entry[250];
    size_t *prev_size;

    gen_entry(entry, file, line);

    if ((prev_size = hashmap_get(mem_active, entry)) != NULL) {
        *prev_size += size;
    } else {
        prev_size = malloc(sizeof(*prev_size));
        *prev_size = size;
    }

    hashmap_set(mem_active, entry, prev_size);

    return malloc(size);
}

void ghh_free(void *ptr, const char *file, const int line) {
    size_t *prev_size;
    char entry[250];

    gen_entry(entry, file, line);

    if ((prev_size = hashmap_remove(mem_active, entry)) != NULL)
        hashmap_set(mem_inactive, entry, prev_size);
    else
        hashmap_set(mem_inactive, entry, NULL);

    free(ptr);
}

void *ghh_realloc(void *old_ptr, size_t size, const char *file, const int line) {
    if (old_ptr == NULL)
        return ghh_alloc(size, file, line);
    else if (size == 0)
        return (ghh_free(old_ptr, "ghh_realloc", 0), NULL);

    return realloc(old_ptr, size);
}
