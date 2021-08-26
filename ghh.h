#ifndef GHH_H
#define GHH_H

#ifndef __STDC_VERSION__
#error "libghh requires C99."
#endif

#include "src/vec.h"
#include "src/hmap.h"
#include "src/time.h"
#include "src/utils.h"

#ifdef GHH_IMPL
#define GHH_VEC_IMPL
#define GHH_HMAP_IMPL
#define GHH_TIME_IMPL
#endif

#ifdef GHH_VEC_IMPL
#include "src/vec.c"
#endif
#ifdef GHH_HMAP_IMPL
#include "src/hmap.c"
#endif
#ifdef GHH_TIME_IMPL
#include "src/time.c"
#endif

#endif
