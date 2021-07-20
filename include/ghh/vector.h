#ifndef GHH_VECTOR_H
#define GHH_VECTOR_H

#define GHH_LIB_INTERNAL
#include <stdlib.h>
#include <ghh/utils.h>
#include <ghh/memcheck.h>
#undef GHH_LIB_INTERNAL

/*
sometimes you want a dynamic array, but using array_t would require a lot of
expensive malloc's, or you're using primitives. vectors are the solution for
this.

- vectors are 3-4x faster than arrays for a pushing/popping a list of primitives
    - vectors also require no casting on data access
- vectors are slightly slower than arrays for pointers
    - since array_t's have more functionality, might as well use them anyways
- vectors are very slow with larger structs, lots of data copying
    - you can make an array of pointers, but you might as well just use an array_t
*/

// example usage
#if 0

float *v;
int i;

VECTOR_ALLOC(v, 5); // allocate v with size of 5

printf("initial size: %lu\n", VECTOR_ALLOC_SIZE(v));

// add 10 items (v will expand)
for (i = 0; i < 10; ++i)
    VECTOR_PUSH(v, (float)i / 10);

printf("biggest size: %lu\n", VECTOR_ALLOC_SIZE(v));

// access and print items
for (i = 0; i < VECTOR_SIZE(v); ++i)
    printf("item %d: %f\n", i, v[i]);

// pop items, (v will shrink)
for (i = 0; i < VECTOR_SIZE(v); ++i)
    VECTOR_POP(v);

printf("back to initial size: %lu\n", VECTOR_ALLOC_SIZE(v));

VECTOR_FREE(v); // cleanup

#endif

// interface
#define VECTOR_DATA(v) ((ghh_vector_data_t *)v - 1)
#define VECTOR_SIZE(v) VECTOR_DATA(v)->size
#define VECTOR_ALLOC_SIZE(v) VECTOR_DATA(v)->alloc_size

#define VECTOR_ALLOC(vec, initial_size) (\
    vec = ghh_vector_create(initial_size, sizeof(*vec))\
)

#define VECTOR_FREE(vec) FREE(VECTOR_DATA(vec))

#define VECTOR_CLEAR(vec) do {\
    size_t min_size = VECTOR_DATA(vec)->min_size;\
    VECTOR_FREE(vec);\
    VECTOR_ALLOC(vec, min_size);\
} while (0)

#define VECTOR_PUSH(vec, item) (\
    vec = ghh_vector_check_expand(vec),\
    vec[VECTOR_SIZE(vec)++] = (item)\
)

// this is the only consistent way I've found to prevent POP() from triggering
// warnings when the value of POP is unused
#pragma GCC system_header
#define VECTOR_POP(vec) (\
    --VECTOR_SIZE(vec),\
    vec = ghh_vector_check_shrink(vec),\
    vec[VECTOR_SIZE(vec)]\
)

// internal, don't touch this stuff
#define GHH_MIN_VECTOR_SIZE 8

typedef struct ghh_vector_data {
    size_t size, min_size, alloc_size;
    size_t item_size;
} ghh_vector_data_t;

static inline void *ghh_vector_create(size_t initial_size, size_t item_size) {
    ghh_vector_data_t *vec;

    initial_size = MAX(GHH_MIN_VECTOR_SIZE, initial_size);

    vec = MALLOC(sizeof(ghh_vector_data_t) + (initial_size * item_size));
    ++vec;

    VECTOR_SIZE(vec) = 0;
    VECTOR_DATA(vec)->min_size = VECTOR_ALLOC_SIZE(vec) = initial_size;
    VECTOR_DATA(vec)->item_size = item_size;

    return vec;
}

static inline void *ghh_vector_check_expand(void *vec) {
    if (VECTOR_SIZE(vec) == VECTOR_ALLOC_SIZE(vec)) {
        VECTOR_ALLOC_SIZE(vec) <<= 1;

        size_t new_size = VECTOR_ALLOC_SIZE(vec) * VECTOR_DATA(vec)->item_size;
        new_size += sizeof(ghh_vector_data_t);

        vec = (ghh_vector_data_t *)REALLOC(VECTOR_DATA(vec), new_size) + 1;
    }

    return vec;
}

static inline void *ghh_vector_check_shrink(void *vec) {
    if (VECTOR_SIZE(vec) < (VECTOR_ALLOC_SIZE(vec) >> 1)
     && VECTOR_ALLOC_SIZE(vec) > VECTOR_DATA(vec)->min_size) {
        VECTOR_ALLOC_SIZE(vec) >>= 1;

        size_t new_size = VECTOR_ALLOC_SIZE(vec) * VECTOR_DATA(vec)->item_size;
        new_size += sizeof(ghh_vector_data_t);

        vec = (ghh_vector_data_t *)REALLOC(VECTOR_DATA(vec), new_size) + 1;
    }

    return vec;
}

#endif
