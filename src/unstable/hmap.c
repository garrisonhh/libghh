#include <stdlib.h>
#include <ghh_unstable/ghh.h>

// fnv-1a hash function (http://isthe.com/chongo/tech/comp/fnv/)
static hash_t hash_str(char *str) {
    hash_t hash = FNV_BASIS;

    while (*str)
        hash = (hash ^ *str++) * FNV_PRIME;

    return hash;
}

static void put_key(hmap_t *hmap, hkey_t hkey, void *value);

static void rehash(hmap_t *hmap, size_t new_cap) {
    hkey_t *old_keys = hmap->keys;
    void **old_values = hmap->values;
    size_t old_cap = hmap->cap;

    hmap->cap = new_cap;
    hmap->values = malloc(hmap->cap * sizeof(*hmap->values));
    hmap->keys = calloc(hmap->cap, sizeof(*hmap->keys));
    hmap->size = 0;

    for (size_t i = 0; i < old_cap; ++i) {
        if (old_keys[i].hash) {
            hkey_t new_hkey = {
                old_keys[i].hash,
                old_keys[i].hash % hmap->cap
            };

            put_key(hmap, new_hkey, old_values[i]);
        }
    }
}

static inline void alloc_slot(hmap_t *hmap) {
    if (++hmap->size > hmap->cap >> 1)
        rehash(hmap, hmap->cap << 1);
}

static inline void free_slot(hmap_t *hmap) {
    if (hmap->size-- < hmap->cap >> 2 && hmap->cap > hmap->min_cap)
        rehash(hmap, hmap->cap >> 1);
}

static void put_key(hmap_t *hmap, hkey_t hkey, void *value) {
    size_t index = hkey.index;

    // find suitable bucket
    while (hmap->keys[index].hash) {
        if (hmap->keys[index].hash == hkey.hash) {
            // found matching bucket
            if (hmap->keys[index].steps == hkey.steps) {
                // seeing if modifying old steps count is necessary,
                // I don't think it should be?
                printf("steps diff\n");
                hmap->keys[index].steps = hkey.steps;
            }

            hmap->values[hkey.index] = value;
            return;
        }

        index = (index + 1) % hmap->cap;
        ++hkey.steps;
    }

    // found empty bucket
    hmap->keys[index] = hkey;
    hmap->values[index] = value;

    alloc_slot(hmap);
}

static void *get_key(hmap_t *hmap, hkey_t hkey) {
    ; // TODO

    return NULL;
}

void hmap_make_internal(struct ghh_hmap_cfg cfg) {
    hmap_t *hmap = cfg.hmap;

    hmap->size = 0;
    hmap->cap = hmap->min_cap = MAX(cfg.init_cap, HMAP_MIN_CAP);

    hmap->values = malloc(hmap->cap * sizeof(*hmap->values));
    hmap->keys = calloc(hmap->cap, sizeof(*hmap->keys));
}

void hmap_kill(hmap_t *hmap) {
    free(hmap->values);
    free(hmap->keys);
}

void hmap_puts(hmap_t *hmap, char *key, void *value) {
    hkey_t hkey = {
        .hash = hash_str(key),
        .index = hkey.hash % hmap->cap
    };

    put_key(hmap, hkey, value);
}

void *hmap_gets(hmap_t *hmap, char *key) {
    hash_t hash = hash_str(key);
    size_t index = hash % hmap->cap;

    while (hmap->keys[index].hash) {
        if (hmap->keys[index].hash == hash)
            return hmap->values[index];

        index = (index + 1) % hmap->cap;
    }

    return NULL;
}

void hmap_print(hmap_t *hmap) {
    printf("--- hashmap ---\n");

    for (size_t i = 0; i < hmap->cap; ++i) {
        hkey_t *key = &hmap->keys[i];

        printf(
            "%3zu : %016lX | %-6zu | %-6zu | %p\n",
            i, key->hash, key->index, key->steps, hmap->values[i]
        );
    }
}
