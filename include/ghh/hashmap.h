#ifndef GHH_HASHMAP_H
#define GHH_HASHMAP_H

#include <stddef.h>
#include <stdbool.h>

typedef struct ghh_hashmap hashmap_t;
typedef struct ghh_hmapiter hmapiter_t;

// instancing
// key_size < 0: hashes as a string
// key_size > 0: hashes a type where key_size == sizeof(type)
hashmap_t *hashmap_create(size_t initial_size, int key_size, bool copy_keys);
void hashmap_destroy(hashmap_t *, bool destroy_values);

// data access
size_t hashmap_size(hashmap_t *);

// ops
void *hashmap_get(hashmap_t *, const void *key);
void *hashmap_set(hashmap_t *, const void *key, const void *value);
void *hashmap_remove(hashmap_t *, const void *key);

void hashmap_debug(hashmap_t *); // TODO REMOVE

// hashmap iterator

/* usage:
hmapiter_t *iterator = hmapiter_create(hashmap);
void *value;

while (hmapiter_next(iterator, &value))
    ; //do stuff ...

free(iterator);
*/
// hmapiter_t is reset on stop, so can be reused as many times as you want
// hmapiter_t does not have to be recreated when hashmap is changed, but it
// will not work as intended if you edit hashmap during iteration.

hmapiter_t *hmapiter_create(hashmap_t *);
// free() to destroy

void hmapiter_reset(hmapiter_t *);
// returns whether not done
bool hmapiter_next(hmapiter_t *, void **out_key, void **out_value);

#endif
