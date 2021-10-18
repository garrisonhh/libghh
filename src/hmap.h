#ifndef GHH_HMAP_H
#define GHH_HMAP_H

#include <stdint.h>

#if INTPTR_MAX == INT64_MAX
// 64 bit
typedef uint64_t hash_t;
#define FNV_PRIME 0x00000100000001b3
#define FNV_BASIS 0xcbf29ce484222325
#else
// 32 bit
typedef uint32_t hash_t;
#define FNV_PRIME 0x01000193a
#define FNV_BASIS 0x811c9dc5
#endif

#ifndef HMAP_MIN_CAP
#define HMAP_MIN_CAP 8
#endif

typedef struct ghh_hnode {
    void *value;
    hash_t hash; // 0 if unoccupied
    size_t index, steps; // intended loc, slots removed from intended loc
} hnode_t;

// hmap assumes that keys that hash to the same value are the same, there's only
// a ridiculously tiny chance fnv-1a will produce the same hash twice
typedef struct ghh_hmap {
    hnode_t *nodes;
    size_t size, cap, min_cap;
} hmap_t;

// internal
struct ghh_hmap_cfg {
    hmap_t *hmap;
    size_t init_cap;
};

hash_t hash_str(char *str);
hash_t hash_bytes(uint8_t *bytes, size_t size);
void hmap_make_internal(struct ghh_hmap_cfg);
void hmap_put_internal(hmap_t *, hnode_t *node);
void *hmap_get_internal(hmap_t *, hash_t hash);
void hmap_del_internal(hmap_t *hmap, hash_t hash);

// lifetime
#define hmap_make(...) hmap_make_internal((struct ghh_hmap_cfg){__VA_ARGS__})
static inline void hmap_kill(hmap_t *hmap) { free(hmap->nodes); }

// ops
static inline void hmap_puts(hmap_t *hmap, char *key, void *value) {
    hnode_t node = {
        .value = value,
        .hash = hash_str(key),
        .index = node.hash % hmap->cap
    };

    hmap_put_internal(hmap, &node);
}

static inline void *hmap_gets(hmap_t *hmap, char *key) {
    return hmap_get_internal(hmap, hash_str(key));
}

static inline void hmap_dels(hmap_t *hmap, char *key) {
    hmap_del_internal(hmap, hash_str(key));
}

static inline void hmap_putb(
    hmap_t *hmap, uint8_t *bytes, size_t size, void *value
) {
    hnode_t node = {
        .value = value,
        .hash = hash_bytes(bytes, size),
        .index = node.hash % hmap->cap
    };

    hmap_put_internal(hmap, &node);
}

static inline void *hmap_getb(hmap_t *hmap, uint8_t *bytes, size_t size) {
    return hmap_get_internal(hmap, hash_bytes(bytes, size));
}

static inline void hmap_delb(hmap_t *hmap, uint8_t *bytes, size_t size) {
    hmap_del_internal(hmap, hash_bytes(bytes, size));
}

#define hmap_put(hmap, key, value)\
    hmap_putb(hmap, (uint8_t *)&(key), sizeof(key), value)
#define hmap_get(hmap, key)\
    hmap_getb(hmap, (uint8_t *)&(key), sizeof(key))
#define hmap_del(hmap, key)\
    hmap_delb(hmap, (uint8_t *)&(key), sizeof(key))

#ifdef GHH_DEBUG
void hmap_print(hmap_t *);
#endif

#endif
