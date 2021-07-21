#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ghh/hashmap.h>
#include <ghh/utils.h>

#ifdef GHH_MEMCHECK_H
#error "DO NOT INCLUDE <ghh/memcheck.h> IN memcheck.c"
#endif

hashmap_t *entries = NULL; // char * => entry_t *
hashmap_t *pointers = NULL; // void * => char * (key for entries)
bool post_quit;

typedef struct entry {
    // bytes per allocation, current allocations, total number of allocations
    size_t bytes, current, total;
} entry_t;

void memcheck_init() {
    entries = hashmap_create(256, -1, true);
    pointers = hashmap_create(256, sizeof(void *), true);
}

void memcheck_quit() {
    char *key;
    entry_t *entry;
    hmapiter_t *iter;
    size_t width, max_key_width = 0;
    size_t unfreed = 0;

    iter = hmapiter_create(entries);

    // find total unfreed bytes and max file path width
    while (hmapiter_next(iter, (void **)&key, (void **)&entry)) {
        if (entry->current) {
            if ((width = strlen(key)) > max_key_width)
                max_key_width = width;

            unfreed += entry->current * entry->bytes;
        }
    }

    max_key_width = MAX(max_key_width, 40); // for formatting

    // generate status report
    char desc[100], separator[100];
    size_t i;

    sprintf(desc, "%-*s unfreed bytes", max_key_width, "location");

    for (i = 0; desc[i]; ++i)
        separator[i] = '-';

    separator[i] = 0;

    puts(separator);
    puts("ghh memcheck status");
    puts(separator);

    if (unfreed) {
        puts(desc);
        puts(separator);

        while (hmapiter_next(iter, (void **)&key, (void **)&entry)) {
            if (entry->current) {
                printf(
                    "%-*s %ldx%ld\n",
                    max_key_width, key, entry->current, entry->bytes
                );
            }
        }

        puts(separator);
        printf("found %ld unfreed bytes in total.\n", unfreed);
        puts(separator);
    } else {
        printf("all matched memory allocations freed.\n");
    }

    // have to manually free entries and pointer values to prevent segfault on unmatched
    // ghh_free()
    while (hmapiter_next(iter, NULL, (void **)&entry))
        free(entry);

    free(iter);

    HMAP_FOREACH_V(key, pointers)
        free(key);

    hashmap_destroy(entries, false);
    hashmap_destroy(pointers, false);
}

static inline void gen_key(char *str, const char *file, const int line) {
    sprintf(str, "%s:%d", file, line);
}

void *ghh_alloc(size_t size, const char *file, const int line) {
    void *ptr = calloc(1, size);
    entry_t *entry;
    char key[100];

    gen_key(key, file, line);

    // update proper entry
    if (hashmap_may_get(entries, key, (void **)&entry)) {
        ++entry->total;
        ++entry->current;
    } else {
        entry = malloc(sizeof(*entry));

        entry->bytes = size;
        entry->current = entry->total = 1;

        hashmap_set(entries, key, entry);
    }

    // map pointer to entry key
    char *copied = malloc(sizeof(*key) * (strlen(key) + 1));

    strcpy(copied, key);

    hashmap_set(pointers, &ptr, copied);

    return ptr;
}

void ghh_free(void *ptr, const char *file, const int line) {
    char *key;

    if (hashmap_may_get(pointers, &ptr, (void **)&key)) {
        // if segfault from null here, something is broken in pointer mapping
        --((entry_t *)hashmap_get(entries, key))->current;
    } else {
        char unmatched[100];

        gen_key(unmatched, file, line);

        printf("unmatched free\t%18p => %s\n", ptr, unmatched);
    }

    free(ptr);
}

void *ghh_realloc(void *old_ptr, size_t size, const char *file, const int line) {
    // edge cases where realloc acts as malloc or free
    if (old_ptr == NULL)
        return ghh_alloc(size, file, line);
    else if (size == 0)
        return (ghh_free(old_ptr, file, line), NULL);

    // realloc acting as realloc
    void *ptr;

    ptr = ghh_alloc(size, file, line);
    memcpy(ptr, old_ptr, size);
    ghh_free(old_ptr, file, line);

    return ptr;
}
