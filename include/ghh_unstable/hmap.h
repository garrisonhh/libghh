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

void hmap_make_internal(struct ghh_hmap_cfg);

// lifetime
#define hmap_make(...) hmap_make_internal((struct ghh_hmap_cfg){__VA_ARGS__})
static inline void hmap_kill(hmap_t *hmap) { free(hmap->nodes); }

// ops
void hmap_puts(hmap_t *, char *key, void *value);
void *hmap_gets(hmap_t *, char *key);
void hmap_dels(hmap_t *, char *key);
// TODO void hmap_put(); for any type with hash_any(char *, size_t size)

void hmap_print(hmap_t *); // DEBUGGING

#endif