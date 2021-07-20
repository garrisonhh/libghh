#ifndef GHH_MEMCHECK_H
#define GHH_MEMCHECK_H

// yes, this header needs to know about stdlib.h
#include <stdlib.h>

#ifndef DEBUG
// #define GHH_MEMCHECK_ENABLED before including this header
// memcheck disables itself outside of debug builds
#undef GHH_MEMCHECK_ENABLED
#endif

#ifdef GHH_MEMCHECK_ENABLED

void memcheck_init(void);
void memcheck_quit(void); // spits log to stdout

void *ghh_alloc(size_t size, const char *file, const int line);
void *ghh_realloc(void *old_ptr, size_t size, const char *file, const int line);
void ghh_free(void *ptr, const char *file, const int line);

#define malloc(size) ghh_alloc(size, __FILE__, __LINE__)
#define calloc(size, item_size) ghh_alloc((size) * (item_size), __FILE__, __LINE__)
#define realloc(ptr, size) ghh_realloc(ptr, size, __FILE__, __LINE__)
#define free(ptr) ghh_free(ptr, __FILE__, __LINE__)

#else

#define memcheck_init()
#define memcheck_quit()

#endif

#endif
