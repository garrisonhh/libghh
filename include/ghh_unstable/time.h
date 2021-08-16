#ifndef GHH_TIME_H
#define GHH_TIME_H

#include <stddef.h>
#include <sys/time.h> // mingw supplies this header

double time_get(void);
void timeit_start(void);
void timeit_end(const char *message);

// gtimer ======================================================================

typedef struct ghh_timer {
	double last_tick, this_tick;
	double tick;
	double *tracked;
	size_t len_tracked, tracked_idx;
} gtimer_t;

void gtimer_make(gtimer_t *, size_t len_tracked);
void gtimer_kill(gtimer_t *);

void gtimer_tick(gtimer_t *);
void gtimer_pop_tick(gtimer_t *);
double gtimer_get_tick(gtimer_t *); // time elapsed between last ticks
double gtimer_get_fps(gtimer_t *);
double gtimer_get_avg_tick(gtimer_t *);

#endif
