#ifndef GHH_VECTOR_H
#define GHH_VECTOR_H

#include <stdlib.h>
#include <ghh/utils.h>

/*
sometimes you want a dynamic array, but using an array_t is overkill or
inconvenient with all of the memory management, etc. vector.h is a solution
for those cases.
*/

// example usage
#if 0

float *v;
int i;

VECTOR_ALLOC(v, 5); // initialize v with size of 5

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

#define VECTOR_FREE(vec) free(VECTOR_DATA(vec))

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

    vec = malloc(sizeof(ghh_vector_data_t) + (initial_size * item_size));
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

        vec = (ghh_vector_data_t *)realloc(VECTOR_DATA(vec), new_size) + 1;
    }

    return vec;
}

static inline void *ghh_vector_check_shrink(void *vec) {
    if (VECTOR_SIZE(vec) < (VECTOR_ALLOC_SIZE(vec) >> 1)
     && VECTOR_ALLOC_SIZE(vec) > VECTOR_DATA(vec)->min_size) {
        VECTOR_ALLOC_SIZE(vec) >>= 1;

        size_t new_size = VECTOR_ALLOC_SIZE(vec) * VECTOR_DATA(vec)->item_size;
        new_size += sizeof(ghh_vector_data_t);

        vec = (ghh_vector_data_t *)realloc(VECTOR_DATA(vec), new_size) + 1;
    }

    return vec;
}

#endif
