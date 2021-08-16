#ifndef GHH_VEC_H
#define GHH_VEC_H

#include <stdlib.h>

#ifndef VEC_MIN_CAP
#define VEC_MIN_CAP 4
#endif
#ifndef VEC_LINEAR_GROWTH
#define VEC_LINEAR_GROWTH 64
#endif

// if set, insert and remove will preserve order
const unsigned VEC_ORDERED = 1 << 0;
// if set, push and pop will grow/shrink capacity in VEC_LINEAR_GROWTH size
// chunks rather than by a factor of 2
const unsigned VEC_LINEAR = 1 << 1;

typedef struct ghh_vec {
    void **data;
    size_t size, cap, min_cap;
    unsigned flags: 2;
} vec_t;

// exclusively for vec_init macro
struct ghh_vec_init_cfg {
    vec_t *vec;
    size_t init_cap;
    unsigned flags: 2;
};

void vec_make_internal(struct ghh_vec_init_cfg);
#define vec_make(...) vec_make_internal((struct ghh_vec_init_cfg){__VA_ARGS__})
static inline void vec_kill(vec_t *vec) { free(vec->data); }

// LIFO stack ops
void vec_push(vec_t *, void *item);
void *vec_pop(vec_t *);
static inline void *vec_peek(vec_t *vec) { return vec->data[vec->size - 1]; }

// dynamic array ops
void vec_insert(vec_t *, size_t idx, void *item);
void vec_remove(vec_t *, size_t idx);

static inline void vec_clear(vec_t *vec) {
    if (vec->size) {
        vec_kill(vec);
        vec_make(.vec = vec, .init_cap = vec->min_cap, .flags = vec->flags);
    }
}

// macros
#define vec_foreach(item, vec)\
    for (size_t vec_idx = 0;\
        (item = (vec)->data[vec_idx], vec_idx < (vec)->size);\
        ++vec_idx)

#define vec_map(vec, func)\
    do {\
        void *vec_item;\
        vec_foreach(vec_item, vec)\
            func(vec_item);\
    } while (0)

#define vec_qsort(vec, compare)\
    qsort(vec->data, vec->size, sizeof(*vec->data), compare)

#endif
