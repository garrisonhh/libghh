#include <stdbool.h>
#include <string.h>
#include <ghh_unstable/ghh.h>

// vec =========================================================================

void vec_make_internal(struct ghh_vec_init_cfg cfg) {
    SANITY_CHECK(cfg.vec, "must pass a valid vec pointer to vec_init.\n");

    vec_t *vec = cfg.vec;

    vec->size = 0;
    vec->min_cap = vec->cap = MAX(cfg.init_cap, VEC_MIN_CAP);
    vec->flags = cfg.flags;

    vec->data = malloc(vec->cap * sizeof(*vec->data));
}

static void alloc_one(vec_t *vec) {
    if (vec->size + 1 > vec->cap) {
        vec->cap = vec->flags & VEC_LINEAR
            ? vec->cap + VEC_LINEAR_GROWTH
            : vec->cap * 2;
        vec->data = realloc(vec->data, vec->cap * sizeof(*vec->data));
    }
}

static void free_one(vec_t *vec) {
    SANITY_CHECK(vec->size, "can't shrink vec of zero size.\n");

    if (vec->cap > vec->min_cap) {
        bool resize = false;

        if (vec->flags & VEC_LINEAR) {
            if ((resize = vec->size < vec->cap - VEC_LINEAR_GROWTH * 2))
                vec->cap -= VEC_LINEAR_GROWTH;
        } else {
            if ((resize = vec->size < vec->cap / 4))
                vec->cap /= 2;
        }

        if (resize)
            vec->data = realloc(vec->data, vec->cap * sizeof(*vec->data));
    }
}

void vec_push(vec_t *vec, void *item) {
    alloc_one(vec);

    vec->data[vec->size++] = item;
}

void *vec_pop(vec_t *vec) {
    void *item = vec->data[vec->size - 1];

    free_one(vec);
    --vec->size;

    return item;
}

void vec_insert(vec_t *vec, size_t idx, void *item) {
    SANITY_CHECK(
        idx < vec->size + 1,
        "attempted to insert past the end of vec + 1.\n"
    );

    alloc_one(vec);

    if (idx < vec->size) {
        if (vec->flags & VEC_ORDERED) {
            // shift all elements over to make room for item
            memmove(vec->data + idx + 1, vec->data + idx, vec->size++ - idx);
        } else {
            // move previous item to end of vec to make room for item
            vec_push(vec, vec->data[idx]);
        }
    }

    vec->data[idx] = item;
}

void vec_remove(vec_t *vec, size_t idx) {
    SANITY_CHECK(idx < vec->size, "attempted to remove past the end of vec.\n");

    if (vec->flags & VEC_ORDERED) {
        // shift all elements over to remove item
        memmove(vec->data + idx, vec->data + idx + 1, vec->size - 1 - idx);
    } else {
        // move item at end of vec to this item to make room for item
        vec->data[idx] = vec->data[vec->size - 1];
    }

    free_one(vec);
    --vec->size;
}

// fvec ========================================================================

#define FVEC_BYTES(item_size, cap)\
    (sizeof(struct ghh_fvec) + (item_size) * (cap))

void *fvec_make_internal(struct ghh_fvec_init_cfg cfg) {
    cfg.init_cap = MAX(cfg.init_cap, VEC_MIN_CAP);

    struct ghh_fvec *fv;

    fv = malloc(FVEC_BYTES(cfg.item_size, cfg.init_cap));
    fv->size = 0;
    fv->cap = fv->min_cap = cfg.init_cap;
    fv->flags = cfg.flags;

    return fv + 1;
}

void *fvec_alloc_one_internal(size_t item_size, struct ghh_fvec *fv) {
    if (fv->size + 1 > fv->cap) {
        fv->cap = fv->flags & VEC_LINEAR
            ? fv->cap + VEC_LINEAR_GROWTH
            : fv->cap * 2;
        fv = realloc(fv, FVEC_BYTES(item_size, fv->cap));
    }

    return fv + 1;
}

void *fvec_free_one_internal(size_t item_size, struct ghh_fvec *fv) {
    SANITY_CHECK(fv->size, "can't shrink fvec of zero size.\n");

    if (fv->cap > fv->min_cap) {
        bool resize = false;

        // for the macro to work, the last element must always remain valid
        if (fv->flags & VEC_LINEAR) {
            if ((resize = fv->size < fv->cap - VEC_LINEAR_GROWTH * 2))
                fv->cap -= VEC_LINEAR_GROWTH;
        } else {
            if ((resize = fv->size < fv->cap / 4))
                fv->cap /= 2;
        }

        if (resize)
            fv = realloc(fv, FVEC_BYTES(item_size, fv->cap));
    }

    return fv + 1;
}
