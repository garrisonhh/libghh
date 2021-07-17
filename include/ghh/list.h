#ifndef GHH_LIST_H
#define GHH_LIST_H

#include <stddef.h>
#include <stdbool.h>

typedef struct ghh_list list_t;
typedef struct ghh_listiter listiter_t;

// list
list_t *list_create(void);
void list_destroy(list_t *, bool destroy_values);

// data access
size_t list_size(list_t *);
void *list_get(list_t *, size_t index);
void *list_get_root(list_t *);
void *list_get_tip(list_t *);

// stack/queue ops
void list_push(list_t *, void *item);
void list_append(list_t *, void *item);
void *list_pop(list_t *);
void *list_peek(list_t *);

// directly compares pointers
void *list_remove(list_t *, void *item);
void list_merge(list_t *, list_t *other);

// list iterator
listiter_t *list_iter_create(list_t *);

void list_iter_reset(listiter_t *);
void *list_iter_next(listiter_t *); // returns NULL when done

#endif
