#ifndef GHH_TIME_H
#define GHH_TIME_H

#include <stddef.h>
#include <sys/time.h> // works through mingw on windows

typedef struct ghh_timer gtimer_t;

double time_get(void);
void timeit_start(void);
void timeit_end(const char *message);

gtimer_t *gtimer_create(size_t len_tracked);
void gtimer_destroy(gtimer_t *);

void gtimer_tick(gtimer_t *);
void gtimer_pop_tick(gtimer_t *);
double gtimer_get_tick(gtimer_t *); // time elapsed between last ticks
double gtimer_get_fps(gtimer_t *);
double gtimer_get_avg_tick(gtimer_t *);

#endif
