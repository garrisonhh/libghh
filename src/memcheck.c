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
long long unsigned int current_alloc = 0;
long long unsigned int max_alloc = 0;

typedef struct entry {
    // bytes per allocation, current allocations, total number of allocations
    size_t bytes, current, total;
} entry_t;

void memcheck_init() {
    entries = hashmap_create(256, -1, true);
    pointers = hashmap_create(256, sizeof(void *), true);
}

void memcheck_quit(bool all) {
    char *key;
    char separator[101];
    entry_t *entry;
    hmapiter_t *iter;
    size_t width, max_key_width = 70;
    size_t unfreed = 0;

    iter = hmapiter_create(entries);

    // find total unfreed bytes and max file path width
    while (hmapiter_next(iter, (void **)&key, (void **)&entry)) {
        if ((width = strlen(key)) > max_key_width)
            max_key_width = width;

        unfreed += entry->current * entry->bytes;
    }

    max_key_width = MAX(max_key_width, 40); // for formatting

    // generate status report
    memset(separator, '-', ARRAY_LEN(separator));
    separator[ARRAY_LEN(separator) - 1] = 0;

    puts(separator);

    // total allocations
    if (all) {
        printf("%-*s allocations x bytes\n", max_key_width, "PROPERLY FREED");
        puts(separator);

        while (hmapiter_next(iter, (void **)&key, (void **)&entry)) {
            printf(
                "%-*s %11ld x %ld\n",
                max_key_width, key, entry->total, entry->bytes
            );
        }

        puts(separator);
    }

    // leaked memory
    if (unfreed) {
        printf("%-*s allocations x bytes\n", max_key_width, "MEMORY LEAKS");
        puts(separator);

        while (hmapiter_next(iter, (void **)&key, (void **)&entry)) {
            if (entry->current) {
                printf(
                    "%-*s %11ld x %ld\n",
                    max_key_width, key, entry->total, entry->bytes
                );
            }
        }

        puts(separator);
        printf("%ld unfreed bytes found in total.\n", unfreed);
    } else {
        printf("all matched memory allocations were properly freed.\n");
    }

    puts(separator);

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

    // update global tracking
    current_alloc += size;

    if (current_alloc > max_alloc)
        max_alloc = current_alloc;

    return ptr;
}

void ghh_free(void *ptr, const char *file, const int line) {
    char *key;

    if (hashmap_may_get(pointers, &ptr, (void **)&key)) {
        // if segfault from null here, something is broken in pointer mapping
        entry_t *entry = hashmap_get(entries, key);

        --entry->current;

        current_alloc -= entry->bytes;
    } else {
        char unmatched[100];

        gen_key(unmatched, file, line);
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

    // TODO log reallocated size somehow!
    ptr = realloc(old_ptr, size);

    return ptr;
}
