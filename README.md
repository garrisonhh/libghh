# ghh-lib

data structures and utilities I made and use for C99. 

add to a premake project using `include "PATH_TO_GHH_LIB_DIR"` and then `links { "ghh" }`

# usage

common conventions
- struct members `size_t max_size, size, min_size;` specify the max size (aka number of items currently allocated for), number of actual items stored, and the minimum allocated size (to prevent unnecessary realloc calls when you can estimate size)
- data structures preserve all data by default

## array

```c
struct array_t {
	void **items;
	size_t max_size, size, min_size;
}
typedef struct array_t array_t;

array_t *array_create(void);
void array_destroy(array_t *, bool destroy_values);

void array_push(array_t *, void *item);
void *array_pop(array_t *);
void *array_peek(array_t *);
void *array_del(array_t *, int index);
void array_remove(array_t *, void *item);
void array_clear(array_t *, bool destroy_values);
void array_qsort(array_t *, int (*compare)(const void *, const void *));
```

- reallocs its memory when pushing and popping

## hashmap

```c
typedef struct hashbucket_t hashbucket_t;
typedef struct hashmap_t hashmap_t;
typedef struct hashmap_iter_t hashmap_iter_t;

struct hashmap_t {
	hashbucket_t *buckets;
	size_t max_size, size, min_size;
	hashmap_funcs_t funcs;
};

// hashmap
hashmap_t *hashmap_create(size_t initial_size, hashable_e hash_type);
void hashmap_destroy(hashmap_t *, bool destroy_values);

void *hashmap_get(hashmap_t *, const void *key);
void hashmap_set(hashmap_t *, const void *key, const void *value);
void *hashmap_remove(hashmap_t *, const void *key);

void hashmap_print(hashmap_t *, size_t, size_t);

// hashmap iterator
hashmap_iter_t *hashmap_iter_create(hashmap_t *);

void hashmap_iter_reset(hashmap_iter_t *);
void *hashmap_iter_next(hashmap_iter_t *); // returns NULL when done
```

```c
// hashable enum (located in hashable.h)
enum hashable_e {
	HASH_UNSIGNED,
	HASH_STRING,
	HASH_V3I,
};
typedef enum hashable_e hashable_e;
```
- hashmaps handle key types through the `hashable_e` enum entry
- keys are robust: hashmap copies the key data from the key and only compares data, not pointers
  - the downside of this is that long keys will also be copied, so don't use massive keys
- `hashmap_remove()` returns the `value` pointer

## heap

```c
struct heap_t {
	void **items;
	size_t max_size, size;
	int (*compare)(const void *, const void *);
};
typedef struct heap_t heap_t;

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
#define LIST_FOREACH(node, list) for (node = list->root; node != NULL; node = node->next)

struct list_node_t {
	void *item;
	struct list_node_t *next;
};
typedef struct list_node_t list_node_t;

struct list_t {
	list_node_t *root, *tip;
	size_t size;
};
typedef struct list_t list_t;

list_t *list_create();
void list_destroy(list_t *, bool destroy_values);

void list_push(list_t *, void *item);
void *list_pop(list_t *);
void *list_peek(list_t *);
void list_append(list_t *, void *item);

void *list_get(list_t *, size_t index);

// directly compares pointers
void *list_remove(list_t *, void *item);
void list_merge(list_t *, list_t *other);
```

- using `push()` and `pop()` will act as a stack by default (push to/pop from root)
	- use `append()` instead of `push()` for queue functionality
- `list_merge()` pops all items from the second list and appends them to the first, but does not destroy it
