#ifndef GHH_VEC_H
#define GHH_VEC_H

#include <stdlib.h>

// configuration is shared by vec and fvec
#ifndef VEC_MIN_CAP
#define VEC_MIN_CAP 4
#endif

// vec =========================================================================
// dynamic array implementation using void *

// internal stuff
struct ghh_vec_cfg {
    struct ghh_vec *vec;
    size_t init_cap;
};

void vec_make_internal(struct ghh_vec_cfg);

// external stuff
typedef struct ghh_vec {
    void **data;
    size_t size, cap, min_cap;
} vec_t;

// lifetime
#define vec_make(...) vec_make_internal((struct ghh_vec_cfg){__VA_ARGS__})
static inline void vec_kill(vec_t *vec) { free(vec->data); }

// LIFO stack ops
void vec_push(vec_t *, void *item);
void *vec_alloc(vec_t *);
void *vec_pop(vec_t *);
static inline void *vec_peek(vec_t *vec) { return vec->data[vec->size - 1]; }

// dynamic array ops
void vec_insert(vec_t *, size_t idx, void *item);
void vec_remove(vec_t *, size_t idx);
void vec_insert_ordered(vec_t *, size_t idx, void *item);
void vec_remove_ordered(vec_t *, size_t idx);

static inline void vec_clear(vec_t *vec) {
    if (vec->size) {
        vec_kill(vec);
        vec_make(vec, .init_cap = vec->min_cap);
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
// dynamic array implementation using fat pointers + macros (fvec == fat vector)

// internal stuff
struct ghh_fvec {
    size_t size, cap, min_cap;
};

struct ghh_fvec_cfg {
    size_t item_size, init_cap;
    unsigned discard: 1;
};

void *fvec_make_internal(struct ghh_fvec_cfg);
void *fvec_alloc_one_internal(size_t item_size, struct ghh_fvec *fv);
void *fvec_free_one_internal(size_t item_size, struct ghh_fvec *fv);
#define fvec_make2(ptr, ...)\
    (ptr) = fvec_make_internal(\
        (struct ghh_fvec_cfg){sizeof(*(ptr)), __VA_ARGS__});

// external stuff
#define fvec(type) type * // just to show programmer intent

// lifetime
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
#define fvec_alloc(ptr) (\
        ptr = fvec_alloc_one_internal(sizeof(*ptr, fvec_data(ptr)),\
        &ptr[fvec_size(ptr)++]\
    )
#define fvec_pop(ptr) (\
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
