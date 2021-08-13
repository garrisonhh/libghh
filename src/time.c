#include <stdlib.h>
#include <ghh/time.h>
#include <ghh/utils.h>
#include <ghh/memcheck.h>

struct timeval LAST_TIMEIT = {0};

double time_get() {
	struct timeval now;

	gettimeofday(&now, NULL);

	return (double)now.tv_sec + ((double)now.tv_usec) / 1000000.0;
}

void timeit_start() {
	gettimeofday(&LAST_TIMEIT, NULL);
}

void timeit_end(const char *message) {
	struct timeval time_now, time_diff;

	gettimeofday(&time_now, NULL);

	time_diff.tv_sec = time_now.tv_sec - LAST_TIMEIT.tv_sec;
	time_diff.tv_usec = time_now.tv_usec - LAST_TIMEIT.tv_usec;

	if (time_diff.tv_usec < 0) {
		--time_diff.tv_sec;
		time_diff.tv_usec += 1000000;
	}

	char us[7];

	us[6] = 0;

	sprintf(us, "%6li", time_diff.tv_usec);

	for (int i = 0; i < 6; i++)
		if (us[i] == ' ')
			us[i] = '0';

	printf("timeit");

	if (message != NULL)
		printf(": %s", message);

	printf(":\t%ld.%ss\n", time_diff.tv_sec, us);

	LAST_TIMEIT = time_now;
}

struct ghh_timer {
	double last_tick, this_tick;
	double tick;
	double *tracked;
	size_t len_tracked, tracked_idx;
};

gtimer_t *gtimer_create(size_t len_tracked) {
	gtimer_t *timer = malloc(sizeof(gtimer_t));

	timer->this_tick = 0;
	timer->len_tracked = len_tracked;
	timer->tracked_idx = 0;

	timer->tracked = malloc(sizeof(double) * timer->len_tracked);

	for (size_t i = 0; i < timer->len_tracked; ++i)
		timer->tracked[i] = 0.0;

	gtimer_tick(timer);

	return timer;
}

void gtimer_destroy(gtimer_t *timer) {
	free(timer->tracked);
	free(timer);
}

void gtimer_tick(gtimer_t *timer) {
	timer->last_tick = timer->this_tick;
	timer->this_tick = timeit_get_time();

	timer->tick = timer->this_tick - timer->last_tick;

	timer->tracked[timer->tracked_idx++] = timer->tick;
	timer->tracked_idx %= timer->len_tracked;
}

void gtimer_pop_tick(gtimer_t *timer) {
	if (!timer->tracked_idx)
		timer->tracked_idx = timer->len_tracked;

	--timer->tracked_idx;
}

double gtimer_get_tick(gtimer_t *timer) {
	return timer->tick;
}

double gtimer_get_fps(gtimer_t *timer) {
	double average = 0;

	for (size_t i = 0; i < timer->len_tracked; ++i)
		average += timer->tracked[i];

	return timer->len_tracked / average;
}

double gtimer_get_avg_tick(gtimer_t *timer) {
	return 1.0 / gtimer_get_fps(timer);
}