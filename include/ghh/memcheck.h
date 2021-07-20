#ifndef GHH_MEMCHECK_H
#define GHH_MEMCHECK_H

#include <stddef.h>

#ifndef DEBUG
// #define GHH_MEMCHECK_ENABLED before including once!
// memcheck disables itself outside of debug builds
#undef GHH_MEMCHECK_ENABLED
#endif

#ifdef GHH_MEMCHECK_ENABLED

void memcheck_init(void);
void memcheck_quit(void); // spits log to stdout

void *ghh_alloc(size_t size, const char *file, const int line);
void *ghh_realloc(void *old_ptr, size_t size, const char *file, const int line);
void ghh_free(void *ptr, const char *file, const int line);

#define gmalloc(size) ghh_alloc(size, __FILE__, __LINE__)
#define gcalloc(size, item_size) ghh_alloc((size) * (item_size), __FILE__, __LINE__)
#define grealloc(ptr, size) ghh_realloc(ptr, size, __FILE__, __LINE__)
#define gfree(ptr) ghh_free(ptr, __FILE__, __LINE__)

#else

#define memcheck_init()
#define memcheck_quit()

#define gmalloc(size) malloc(size);
#define gcalloc(size, item_size) calloc(size, item_size)
#define grealloc(ptr, size) realloc(ptr, size)
#define gfree(ptr) free((ptr))

#endif

#endif
