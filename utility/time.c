#include <assert.h>
#include <time.h>

#include "violet/utility/time.h"

static b8 timespec_diff(const struct timespec * start, const struct timespec * stop,
                        struct timespec * res)
{
	struct timespec mod_start;
	mod_start.tv_nsec = start->tv_nsec;
	mod_start.tv_sec = start->tv_sec;

	if (stop->tv_nsec < mod_start.tv_nsec)
	{
		mod_start.tv_nsec -= 1000000000;
		++mod_start.tv_sec;
	}
	/* C standard requires states that tv_nsec should be [0, 999,999,999] */
	assert(stop->tv_nsec - mod_start.tv_nsec < 1000000000);

	if (stop->tv_sec >= mod_start.tv_sec)
	{
		res->tv_sec = stop->tv_sec - mod_start.tv_sec;
		res->tv_nsec = stop->tv_nsec - mod_start.tv_nsec;
		return true;
	}
	else
		return false;
}

#ifdef __unix__

void vlt_get_time(vlt_time * t)
{
	clock_gettime(CLOCK_MONOTONIC, t);
}

u32 vlt_diff_milli(vlt_time * start, vlt_time * end)
{
	// TODO(rgriege): handle start > end (see timespec_diff)
	return (end->tv_sec * 1000 + end->tv_nsec / 1000000) -
		(start->tv_sec * 1000 + start->tv_nsec / 1000000);
}

void vlt_sleep_milli(u32 milli)
{
	vlt_time t = { .tv_nsec = milli * 1000000 };
	nanosleep(&t, NULL);
}

#elif(_WIN32)

void vlt_get_time(vlt_time * t)
{
	QueryPerformanceCounter(t);
}

u32 vlt_diff_milli(vlt_time * start, vlt_time * end)
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	return (end->QuadPart - start->QuadPart) * 1000  / frequency.QuadPart;
}

void vlt_sleep_milli(u32 milli)
{
	Sleep(milli);
}

#endif

