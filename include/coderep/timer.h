#ifndef CODEREP_TIMER_H
#define CODEREP_TIMER_H

#include <time.h>

static inline void timer_start(struct timespec *timer)
{
	clock_gettime(CLOCK_MONOTONIC, timer);
}

static inline double timer_elapsed(const struct timespec *timer)
{
	struct timespec end;
	clock_gettime(CLOCK_MONOTONIC, &end);
	return (end.tv_sec - timer->tv_sec) + (end.tv_nsec - timer->tv_nsec) / 1000000000.0;
}


#endif
