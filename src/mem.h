#ifndef GHH_MEM_H
#define GHH_MEM_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

// page-based monotonic allocator
typedef struct ghh_mono {
    uint8_t **pages;
    size_t cur_page, pages_cap;
    size_t cur_size, page_bytes;

#ifdef DEBUG
    size_t large_allocs, total_allocs;
#endif
} mono_t;

void mono_make(mono_t *, size_t page_bytes);
void mono_kill(mono_t *);

void *mono_alloc(mono_t *, size_t bytes);
void *mono_copy(mono_t *, void *data, size_t bytes);

#ifdef DEBUG
static inline void mono_debug(mono_t *mono) {
    double page_density = (double)mono->total_allocs
                        / (double)(mono->cur_page + 1);
    double large_percent = (double)mono->large_allocs
                         / (double)mono->total_allocs;

    printf(
        "=== mono allocator debug info ===\n"
        "allocs:       %zu\n"
        "pages:        %zu\n"
        "allocs/page:  %.2lf\n"
        "large/allocs: %.2lf\n",
        mono->total_allocs, mono->cur_page + 1, page_density, large_percent
    );
}
#else
#define mono_debug(...)
#endif

#endif
