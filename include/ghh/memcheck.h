#ifndef GHH_MEMCHECK_H
#define GHH_MEMCHECK_H

#include <stddef.h>

#ifndef DEBUG
// memcheck is disabled outside of debug mode
#undef GHH_MEMCHECK_ENABLED
#endif

#ifdef GHH_MEMCHECK_ENABLED

void memcheck_init(void);
void memcheck_quit(void); // spits log to stdout

void *ghh_alloc(size_t size, const char *file, const int line);
void *ghh_realloc(void *old_ptr, size_t size, const char *file, const int line);
void ghh_free(void *ptr, const char *file, const int line);

#define MALLOC(size) ghh_alloc(size, __FILE__, __LINE__)
#define CALLOC(size, item_size) MALLOC((size) * (item_size))
#define REALLOC(ptr, size) ghh_realloc(ptr, size, __FILE__, __LINE__)
#define FREE(ptr) ghh_free(ptr, __FILE__, __LINE__)

#else

#define memcheck_init()
#define memcheck_quit()

#define MALLOC(size) malloc(size);
#define CALLOC(size, item_size) calloc(size, item_size)
#define REALLOC(ptr, size) realloc(ptr, size)
#define FREE(ptr) free(ptr)

#endif

#endif
