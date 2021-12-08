#include <stdio.h>
#include <stdlib.h>

#include "data.h"

void mono_make(mono_t *mono, size_t page_bytes) {
#ifndef GHH_MEM_INIT_CAP
#define GHH_MEM_INIT_CAP 8
#endif
    *mono = (mono_t){0};

    mono->page_bytes = page_bytes;
    mono->pages_cap = GHH_MEM_INIT_CAP;
    mono->pages = malloc(mono->pages_cap * sizeof(*mono->pages));
    mono->pages[0] = malloc(mono->page_bytes);
}

void mono_kill(mono_t *mono) {
    for (size_t i = 0; i <= mono->cur_page; ++i)
        free(mono->pages[i]);

    free(mono->pages);
}

// allocates a new page and moves cur_page, returning the new page
static void *mono_next_page(mono_t *mono, size_t bytes) {
    if (++mono->cur_page == mono->pages_cap) {
        mono->pages_cap *= 2;
        mono->pages = realloc(mono->pages, mono->pages_cap * sizeof(*mono->pages));
    }

    void *page = mono->pages[mono->cur_page] = malloc(bytes);

    if (!page)
        GG_ERROR("page allocation failed!\n");

    return page;
}

void *mono_alloc(mono_t *mono, size_t bytes) {
    void *ptr;

    if (mono->cur_size + bytes > mono->page_bytes) {
        // new page
        if (bytes > mono->page_bytes) {
            // large allocation (gets a unique page)
            uint8_t *last_page = mono->pages[mono->cur_page];

            ptr = mono_next_page(mono, bytes);

            mono->pages[mono->cur_page - 1] = ptr;
            mono->pages[mono->cur_page] = last_page;

#ifdef DEBUG
            ++mono->large_allocs;
#endif
        } else {
            ptr = mono_next_page(mono, mono->page_bytes);
            mono->cur_size = 0;
        }
    } else {
        // allocate from page
        ptr = mono->pages[mono->cur_page] + mono->cur_size;
        mono->cur_size += bytes;
    }

#ifdef DEBUG
    ++mono->total_allocs;
#endif

    return ptr;
}
