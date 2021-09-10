#ifndef GHH_H
#define GHH_H

#ifndef __STDC_VERSION__
#error "libghh requires C99."
#endif

#if INTPTR_MAX == INT64_MAX
#define GHH_64BIT
#else
#define GHH_32BIT
#endif

#include "src/vec.h"
#include "src/hmap.h"
#include "src/time.h"
#include "src/utils.h"

#ifdef GHH_IMPL
#include "src/vec.c"
#include "src/hmap.c"
#include "src/time.c"
#endif

#endif
