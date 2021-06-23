#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>
#include <stdbool.h>

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

#endif
