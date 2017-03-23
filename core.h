#ifndef VIOLET_CORE_H
#define VIOLET_CORE_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef _WIN32

#define NOMB
#define NOMINMAX
#define NOGDI
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
typedef LARGE_INTEGER timepoint_t;

#else

#include <time.h>
typedef struct timespec timepoint_t;

#endif // _WIN32


/* Types */

typedef bool b8;
typedef unsigned char u8;
typedef char s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t b32;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
typedef float r32;
typedef double r64;

typedef union uintptr
{
	uintptr_t i;
	void *p;
} uintptr;


/* Utility */

#define UNUSED(x) ((void)(x))

#ifndef NDEBUG
#define check(x) assert(x)
#else
#define check(x) x
#endif

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)

#define memswp(a, b, type) \
	do { type tmp = a; a = b; b = tmp; } while(0)

/* Hash */

u32 hash(const char *str);
u32 hashn(const char *str, u32 n);

/* Time */

timepoint_t time_current();
u32         time_diff_milli(timepoint_t start, timepoint_t end);
u32         time_diff_micro(timepoint_t start, timepoint_t end);
void        time_sleep_milli(u32 milli);

/* Log */

#define LOG_STREAM_CAP 3
void  log_add_stream(FILE *fp);
void  log_remove_stream(FILE *fp);
FILE *log__get_stream(u32 idx);
u32   log__stream_cnt();
#define log_write(fmt, ...) \
	do { \
		for (u32 log_idx = 0; log_idx < log__stream_cnt(); ++log_idx) { \
			fprintf(log__get_stream(log_idx), fmt, ##__VA_ARGS__); \
			fputc('\n', log__get_stream(log_idx)); \
			fflush(log__get_stream(log_idx)); \
		} \
	} while (0)
#define log_newline() \
	do { \
		for (u32 log_idx = 0; log_idx < log__stream_cnt(); ++log_idx) \
			fputc('\n', log__get_stream(log_idx)); \
	} while (0)

#endif // VIOLET_CORE_H


/* Implementation */

#ifdef CORE_IMPLEMENTATION

/* Hash (djb2 by dan bernstein) */

u32 hash(const char *str)
{
	u32 hash = 5381;
	int c;
	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;
	return hash;
}

u32 hashn(const char *str, u32 n)
{
	u32 hash = 5381;
	for (u32 i = 0; i < n; ++i)
		hash = ((hash << 5) + hash) + (int)str[i];
	return hash;
}

/* Time */

#ifndef _WIN32

timepoint_t time_current()
{
	timepoint_t t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return t;
}

static
timepoint_t time__diff(timepoint_t start, timepoint_t end)
{
	timepoint_t mod_start, res;

	mod_start.tv_nsec = start.tv_nsec;
	mod_start.tv_sec = start.tv_sec;
	if (end.tv_nsec < mod_start.tv_nsec) {
		mod_start.tv_nsec -= 1000000000;
		++mod_start.tv_sec;
	}

	/* C standard states that tv_nsec should be [0, 999,999,999] */
	assert(end.tv_nsec - mod_start.tv_nsec < 1000000000);
	assert(end.tv_sec >= mod_start.tv_sec);
	res.tv_sec = end.tv_sec - mod_start.tv_sec;
	res.tv_nsec = end.tv_nsec - mod_start.tv_nsec;
	return res;
}

u32 time_diff_milli(timepoint_t start, timepoint_t end)
{
	timepoint_t res = time__diff(start, end);
	return res.tv_sec * 1000 + res.tv_nsec / 1000000;
}

u32 time_diff_micro(timepoint_t start, timepoint_t end)
{
	timepoint_t res = time__diff(start, end);
	return res.tv_sec * 1000000 + res.tv_nsec / 1000;
}

void time_sleep_milli(u32 milli)
{
	timepoint_t t = { .tv_nsec = milli * 1000000 };
	nanosleep(&t, NULL);
}

#else

timepoint_t time_current()
{
	timepoint_t t;
	QueryPerformanceCounter(&t);
	return t;
}

u32 time_diff_milli(timepoint_t start, timepoint_t end)
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	return (end.QuadPart - start.QuadPart) * 1000 / frequency.QuadPart;
}

u32 time_diff_micro(timepoint_t start, timepoint_t end)
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	return (end.QuadPart - start.QuadPart) * 1000000 / frequency.QuadPart;
}

void time_sleep_milli(u32 milli)
{
	Sleep(milli);
}

#endif

/* Log */

FILE *g_log_streams[LOG_STREAM_CAP];
u32 g_log_stream_cnt = 0;

void log_add_stream(FILE *fp)
{
	assert(g_log_stream_cnt < LOG_STREAM_CAP);
	g_log_streams[g_log_stream_cnt++] = fp;
}

void log_remove_stream(FILE *fp)
{
	for (u32 i = 0; i < g_log_stream_cnt; ++i) {
		if (fp == g_log_streams[i]) {
			g_log_streams[i] = g_log_streams[g_log_stream_cnt-1];
			--g_log_stream_cnt;
			return;
		}
	}
	assert(false);
}

FILE *log__get_stream(u32 idx)
{
	assert(idx < g_log_stream_cnt);
	return g_log_streams[idx];
}

u32 log__stream_cnt()
{
	return g_log_stream_cnt;
}

#undef CORE_IMPLEMENTATION
#endif // CORE_IMPLEMENTATION
