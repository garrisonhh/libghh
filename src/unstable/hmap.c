#include <stdlib.h>
#include <ghh_unstable/ghh.h>

// fnv-1a hash function (http://isthe.com/chongo/tech/comp/fnv/)
static hash_t hash_str(char *str) {
    hash_t hash = FNV_BASIS;

    while (*str)
        hash = (hash ^ *str++) * FNV_PRIME;

    return hash;
}

static void put_node(hmap_t *hmap, hnode_t *node);

static void rehash(hmap_t *hmap, size_t new_cap) {
    hnode_t *old_nodes = hmap->nodes;
    size_t old_cap = hmap->cap;

    hmap->cap = new_cap;
    hmap->nodes = calloc(hmap->cap, sizeof(*hmap->nodes));
    hmap->size = 0;

    for (size_t i = 0; i < old_cap; ++i) {
        if (old_nodes[i].hash) {
            old_nodes[i].index = old_nodes[i].hash % hmap->cap;
            old_nodes[i].steps = 0;

            put_node(hmap, &old_nodes[i]);
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

static void put_node(hmap_t *hmap, hnode_t *node) {
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

    alloc_slot(hmap);
}

static void *get_key(hmap_t *hmap, hnode_t *node) {
    ; // TODO

    return NULL;
}

void hmap_make_internal(struct ghh_hmap_cfg cfg) {
    hmap_t *hmap = cfg.hmap;

    hmap->size = 0;
    hmap->cap = hmap->min_cap = MAX(cfg.init_cap, HMAP_MIN_CAP);
    hmap->nodes = calloc(hmap->cap, sizeof(*hmap->nodes));
}

void hmap_puts(hmap_t *hmap, char *key, void *value) {
    hnode_t node = {
        .value = value,
        .hash = hash_str(key),
        .index = node.hash % hmap->cap
    };

    put_node(hmap, &node);
}

void *hmap_gets(hmap_t *hmap, char *key) {
    hash_t hash = hash_str(key);
    size_t index = hash % hmap->cap;

    // iterate through hash chain until match or empty node is found
    while (hmap->nodes[index].hash) {
        if (hmap->nodes[index].hash == hash)
            return hmap->nodes[index].value;

        index = (index + 1) % hmap->cap;
    }

    return NULL;
}

void hmap_dels(hmap_t *hmap, char *key) {
    hash_t hash = hash_str(key);
    size_t index = hash % hmap->cap;
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
}

void hmap_print(hmap_t *hmap) {
    printf("--- hashmap ---\n");

    for (size_t i = 0; i < hmap->cap; ++i) {
        hnode_t *node = &hmap->nodes[i];

        printf(
            "%3zu : %016lX | %-6zu | %-6zu | %p\n",
            i, node->hash, node->index, node->steps, node->value
        );
    }
}
