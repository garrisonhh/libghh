#ifndef GHH_LIST_H
#define GHH_LIST_H

#include <stddef.h>
#include <stdbool.h>

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

#endif
