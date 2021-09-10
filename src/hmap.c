#include <stdlib.h>

#include "hmap.h"

// fnv-1a hash function (http://isthe.com/chongo/tech/comp/fnv/)
hash_t hash_str(char *str) {
    hash_t hash = FNV_BASIS;

    while (*str)
        hash = (hash ^ *str++) * FNV_PRIME;

    return hash;
}

hash_t hash_bytes(uint8_t *bytes, size_t size) {
    hash_t hash = FNV_BASIS;

    for (size_t i = 0; i < size; ++i)
        hash = (hash ^ bytes[i]) * FNV_PRIME;

    return hash;
}

static void hmap_rehash(hmap_t *hmap, size_t new_cap) {
    hnode_t *old_nodes = hmap->nodes;
    size_t old_cap = hmap->cap;

    hmap->cap = new_cap;
    hmap->nodes = calloc(hmap->cap, sizeof(*hmap->nodes));
    hmap->size = 0;

    for (size_t i = 0; i < old_cap; ++i) {
        if (old_nodes[i].hash) {
            old_nodes[i].index = old_nodes[i].hash % hmap->cap;
            old_nodes[i].steps = 0;

            hmap_put_internal(hmap, &old_nodes[i]);
        }
    }

    free(old_nodes);
}

static inline void hmap_alloc_slot(hmap_t *hmap) {
    if (++hmap->size > hmap->cap >> 1)
        hmap_rehash(hmap, hmap->cap << 1);
}

static inline void hmap_free_slot(hmap_t *hmap) {
    if (hmap->size-- < hmap->cap >> 2 && hmap->cap > hmap->min_cap)
        hmap_rehash(hmap, hmap->cap >> 1);
}

void hmap_put_internal(hmap_t *hmap, hnode_t *node) {
    size_t index = node->index;

    // find suitable bucket
    while (hmap->nodes[index].hash) {
        if (hmap->nodes[index].hash == node->hash) {
            // found matching bucket
            hmap->nodes[index].value = node->value;
            return;
        }

        index = (index + 1) % hmap->cap;
        ++node->steps;
    }

    // found empty bucket
    hmap->nodes[index] = *node;

    hmap_alloc_slot(hmap);
}

void *hmap_get_internal(hmap_t *hmap, hash_t hash) {
    size_t index = hash % hmap->cap;

    // iterate through hash chain until match or empty node is found
    while (hmap->nodes[index].hash) {
        if (hmap->nodes[index].hash == hash)
            return hmap->nodes[index].value;

        index = (index + 1) % hmap->cap;

    }

    return NULL;
}

void hmap_del_internal(hmap_t *hmap, hash_t hash) {
    size_t index = hash % hmap->cap;

    // find node
    while (hmap->nodes[index].hash != hash) {
        if (!hmap->nodes[index].hash)
            return; // node doesn't exist

        index = (index + 1) % hmap->cap;
    }

    // replace chain
    size_t last = index, steps = 0;

    while (hmap->nodes[index = (index + 1) % hmap->cap].hash) {
        if (hmap->nodes[index].steps >= ++steps) {
            // found node that is a valid chain replacement
            hmap->nodes[last] = hmap->nodes[index];
            hmap->nodes[last].steps -= steps;
            last = index;
            steps = 0;
        }
    }

    // last node in chain is now a duplicate
    hmap->nodes[last].hash = 0;
    hmap_free_slot(hmap);
}

void hmap_make_internal(struct ghh_hmap_cfg cfg) {
    hmap_t *hmap = cfg.hmap;

    hmap->size = 0;
    hmap->cap = hmap->min_cap = MAX(cfg.init_cap, HMAP_MIN_CAP);
    hmap->nodes = calloc(hmap->cap, sizeof(*hmap->nodes));
}

void hmap_print(hmap_t *hmap) {
    printf("--- nodes ---\n");

    for (size_t i = 0; i < hmap->cap; ++i) {
        hnode_t *node = &hmap->nodes[i];

        printf(
            "%3zu : %016lX | %-6zu | %-6zu | %s\n",
            i, node->hash, node->index, node->steps, (char *)node->value
        );
    }
}
