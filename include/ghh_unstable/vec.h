#ifndef GHH_VEC_H
#define GHH_VEC_H

#include <stdlib.h>

// configuration is shared by vec and fvec

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

#define GHH_NUM_VEC_FLAGS 2

// vec =========================================================================
// dynamic array implementation using void *

typedef struct ghh_vec {
    void **data;
    size_t size, cap, min_cap;
    unsigned flags: GHH_NUM_VEC_FLAGS;
} vec_t;

// used for vec_make macro
struct ghh_vec_init_cfg {
    vec_t *vec;
    size_t init_cap;
    unsigned flags: GHH_NUM_VEC_FLAGS;
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
        vec_make(vec, .init_cap = vec->min_cap, .flags = vec->flags);
    }
}

// utilities
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

// fvec ========================================================================
// dynamic array implementation using fat pointers (fvec == fat vector) + macros

// internal stuff
struct ghh_fvec {
    size_t size, cap, min_cap;
    unsigned flags: GHH_NUM_VEC_FLAGS;
};

struct ghh_fvec_init_cfg {
    size_t item_size, init_cap;
    unsigned flags: GHH_NUM_VEC_FLAGS;
    unsigned discard: 1;
};

void *fvec_make_internal(struct ghh_fvec_init_cfg);
void *fvec_alloc_one_internal(size_t item_size, struct ghh_fvec *fv);
void *fvec_free_one_internal(size_t item_size, struct ghh_fvec *fv);
#define fvec_make2(ptr, ...)\
    (ptr) = fvec_make_internal(\
        (struct ghh_fvec_init_cfg){sizeof(*(ptr)), __VA_ARGS__});

// make/kill
#define fvec(type) type * // just to show programmer intent
#define fvec_make(...) do { fvec_make2(__VA_ARGS__, .discard = 0) } while (0)
#define fvec_kill(ptr) free(fvec_data(ptr))

// getters
#define fvec_data(ptr) ((struct ghh_fvec *)ptr - 1)
#define fvec_size(ptr) (fvec_data(ptr)->size)
#define fvec_cap(ptr) (fvec_data(ptr)->cap)
#define fvec_min_cap(ptr) (fvec_data(ptr)->min_cap)
#define fvec_flags(ptr) (fvec_data(ptr)->flags)

// LIFO stack ops
#define fvec_push(ptr, item)\
    do {\
        ptr = fvec_alloc_one_internal(sizeof(*ptr), fvec_data(ptr));\
        ptr[fvec_size(ptr)++] = item;\
    } while (0)
#define fvec_pop(ptr, item) (\
        ptr = fvec_free_one_internal(sizeof(*ptr), fvec_data(ptr)),\
        ptr[--fvec_size(ptr)]\
    )
#define fvec_peek(ptr) ptr[fvec_size(ptr) - 1]

// dynamic array ops
// TODO

// utilities
#define fvec_foreach(item, ptr)\
    for (size_t fvec_idx = 0;\
        (item = ptr[fvec_idx], fvec_idx < fvec_size(ptr));\
        ++fvec_idx)

#define fvec_map(ptr, func)\
    do {\
        void *start_ptr = ptr;\
        size_t ptr_size = fvec_size(ptr);\
        while (ptr_size--)\
            func(*ptr++);\
        ptr = start_ptr;\
    } while (0)

#define fvec_qsort(ptr, compare)\
    qsort(ptr, fvec_size(ptr), sizeof(*ptr), compare)

#endif
