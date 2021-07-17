# ghh-lib

data structures and utilities I made and use for C99. 

add to a premake project using `include "PATH_TO_GHH_LIB_DIR"` and then `links { "ghh" }`

## array

a dynamic array implementation, I definitely use it more like a stack

```c
typedef struct array array_t;

// instancing
array_t *array_create(size_t initial_size);
void array_destroy(array_t *, bool destroy_values);

// data access
size_t array_size(array_t *);
void *array_get(array_t *, int index);

// stack ops
void array_push(array_t *, void *item);
void *array_pop(array_t *);
void *array_peek(array_t *);

// array ops
void *array_del(array_t *, int index);
void array_clear(array_t *, bool destroy_values);
void array_qsort(array_t *, int (*compare)(const void *, const void *));
```

## hashmap

```c
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

bool hmapiter_next(hmapiter_t *, void **out_key, void **out_value);

#endif
```

## heap

```c
typedef struct heap heap_t;

heap_t *heap_create(int initial_depth, int (*compare)(const void *, const void *));
void heap_destroy(heap_t *, bool destroy_values);

void heap_insert(heap_t *, void *item);
void *heap_pop(heap_t *);
void *heap_peek(heap_t *);
void *heap_replace(heap_t *, void *item);
```

- sorting compare function is applied similarly to qsort, you can use the same functions (strcmp for example)
- be sure to free popped values that you want freed, this is easy to miss

## list

```c
typedef struct list list_t;
typedef struct list_iter list_iter_t;

// list
list_t *list_create(void);
void list_destroy(list_t *, bool destroy_values);

// data access
size_t list_size(list_t *);
void *list_get(list_t *, size_t index);

// stack/queue ops
void list_push(list_t *, void *item);
void list_append(list_t *, void *item);
void *list_pop(list_t *);
void *list_peek(list_t *);

// directly compares pointers
void *list_remove(list_t *, void *item);
void list_merge(list_t *, list_t *other);

// list iterator
list_iter_t *list_iter_create(list_t *);

void list_iter_reset(list_iter_t *);
void *list_iter_next(list_iter_t *); // returns NULL when done
```

- `list_merge()` pops all items from the second list and appends them to the first, but does not destroy the second list
