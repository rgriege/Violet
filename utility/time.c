#include "violet/utility/time.h"

#ifdef __unix__

void vlt_get_time(vlt_time * t)
{
	clock_gettime(CLOCK_MONOTONIC, t);
}

u32 vlt_diff_milli(vlt_time * start, vlt_time * end)
{
	return (end->tv_sec * 1000 + end->tv_nsec / 1000000) -
		(start->tv_sec * 1000 + start->tv_nsec / 1000000);
}

void vlt_sleep_milli(u32 milli)
{
	vlt_time t = { .tv_nsec = milli * 1000000 };
	nanosleep(&t, NULL);
}

#endif
