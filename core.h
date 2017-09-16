#ifndef VIOLET_CORE_H
#define VIOLET_CORE_H

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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


/* Utility macros */

#define UNUSED(x) ((void)(x))
#define NOOP UNUSED(0)

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)

#define countof(x) (sizeof(x) / sizeof((x)[0]))

#define static_assert(cnd, msg) int msg[(cnd) ? 1 : -1]

#ifndef NDEBUG
#define check(x) assert(x)
#else
#define check(x) x
#endif

#define memswp(a, b, type) \
	do { type tmp = a; a = b; b = tmp; } while(0)

#ifndef WIN32
#define thread_local __thread
#else
#define thread_local __declspec(thread)
#endif


/* Types */

typedef bool b8;
typedef uint8_t u8;
typedef int8_t s8;
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

typedef union intptr
{
	intptr_t i;
	void *p;
} intptr;

static_assert(sizeof(r32) == 4, invalid_floating_point_size);
static_assert(sizeof(r64) == 8, invalid_double_size);

/* Memory */

#ifdef VLT_TRACK_MEMORY

void *vmalloc(size_t sz);
void *vcalloc(size_t nmemb, size_t sz);
void *vrealloc(void *ptr, size_t sz);
void  vfree(void *ptr);

void  vlt_mem_generation_advance();
#ifndef VLT_ALLOC_TRACK_CNT
#define VLT_ALLOC_TRACK_CNT 2048
#endif

typedef void(*vlt_mem_dump_callback_t)(void *ptr, u32 gen, void *udata);
void  vlt_mem_dump_file(void *ptr, u32 gen, void *udata);

void  vlt_mem_dump();
void  vlt_mem_dump_current(vlt_mem_dump_callback_t cb, void *udata);

#define vlt__real_malloc(sz)         malloc(sz)
#define vlt__real_calloc(nmemb, sz)  calloc(nmemb, sz)
#define vlt__real_realloc(ptr, sz)   realloc(ptr, sz)
#define vlt__real_free(ptr)          free(ptr)

#define malloc(sz)         vmalloc(sz)
#define calloc(nmemb, sz)  vcalloc(nmemb, sz)
#define realloc(ptr, sz)   vrealloc(ptr, sz)
#define free(ptr)          vfree(ptr)

#else
#define vlt_mem_generation_advance() NOOP
#endif // VLT_TRACK_MEMORY

/* Hash */

u32 hash(const char *str);
u32 hashn(const char *str, u32 n);

/* Time */

timepoint_t time_current();
u32         time_diff_milli(timepoint_t start, timepoint_t end);
u32         time_diff_micro(timepoint_t start, timepoint_t end);
void        time_sleep_milli(u32 milli);

/* Log */

typedef enum log_level
{
	LOG_DEBUG   = 0x1,
	LOG_INFO    = 0x2,
	LOG_WARNING = 0x4,
	LOG_ERROR   = 0x8,
	LOG_FATAL   = 0x10,
	LOG_ALL     = (LOG_DEBUG | LOG_INFO | LOG_WARNING | LOG_ERROR | LOG_FATAL),
	LOG_STDERR  = (LOG_WARNING | LOG_ERROR | LOG_FATAL),
} log_level_t;

typedef void(*logger_t)(void *udata, const char *format, va_list ap);

#define LOG_STREAM_CAP 3

void log_add_stream(log_level_t level, logger_t logger, void *udata);
void log_remove_stream(logger_t logger, void *udata);
#define log_add_file(level, fp) log_add_stream(level, file_logger, fp)
#define log_remove_file(fp) log_remove_stream(file_logger, fp)
#define log_add_std(stdout_level) do { \
		log_add_file(stdout_level, stdout); \
		log_add_file(LOG_STDERR, stderr); \
	} while (0);
#define log_remove_std() do { \
		log_remove_file(stdout); \
		log_remove_file(stderr); \
	} while (0);
void log_level_write(log_level_t level, const char *format, ...);
#define log_debug(fmt, ...) log_level_write(LOG_DEBUG,   fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)  log_level_write(LOG_INFO,    fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...)  log_level_write(LOG_WARNING, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...) log_level_write(LOG_ERROR,   fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...) log_level_write(LOG_FATAL,   fmt, ##__VA_ARGS__)
#define log_write(fmt, ...) log_level_write(LOG_INFO,    fmt, ##__VA_ARGS__)

void file_logger(void *udata, const char *format, va_list ap);

#endif // VIOLET_CORE_H


/* Implementation */

#ifdef CORE_IMPLEMENTATION

#ifdef VLT_TRACK_MEMORY

typedef struct vlt__alloc_node
{
	struct vlt__alloc_node *prev, *next;
	size_t sz, generation;
} vlt__alloc_node_t;

thread_local struct
{
	vlt__alloc_node_t *head, *tail;
	size_t sz, generation;
	size_t current_bytes, peak_bytes, total_bytes;
	size_t total_chunks;
} g_vlt_allocs = {0};

static
void vlt__record_alloc(size_t sz)
{
	g_vlt_allocs.current_bytes += sz;
	if (g_vlt_allocs.peak_bytes < g_vlt_allocs.current_bytes)
		g_vlt_allocs.peak_bytes = g_vlt_allocs.current_bytes;
	g_vlt_allocs.total_bytes += sz;
	++g_vlt_allocs.total_chunks;
}

static
void vlt__record_free(size_t sz)
{
	g_vlt_allocs.current_bytes -= sz;
}

static
void vlt__alloc_node_append(vlt__alloc_node_t *node, size_t sz)
{
	node->sz = sz;
	node->generation = g_vlt_allocs.generation;
	vlt__record_alloc(sz);
	node->next = NULL;
	if (g_vlt_allocs.tail) {
		g_vlt_allocs.tail->next = node;
		node->prev = g_vlt_allocs.tail;
		g_vlt_allocs.tail = node;
	} else {
		g_vlt_allocs.head = node;
		g_vlt_allocs.tail = node;
		node->prev = NULL;
	}
}

#undef malloc
#undef calloc
#undef realloc
#undef free
#define malloc(sz)         vlt__real_malloc(sz)
#define calloc(nmemb, sz)  vlt__real_calloc(nmemb, sz)
#define realloc(ptr, sz)   vlt__real_realloc(ptr, sz)
#define free(ptr)          vlt__real_free(ptr)

void *vmalloc(size_t sz)
{
	vlt__alloc_node_t *node = malloc(sizeof(vlt__alloc_node_t) + sz);
	vlt__alloc_node_append(node, sz);
	return node + 1;
}

void *vcalloc(size_t nmemb, size_t sz)
{
	vlt__alloc_node_t *node = calloc(1, sizeof(vlt__alloc_node_t) + nmemb * sz);
	vlt__alloc_node_append(node, nmemb * sz);
	return node + 1;
}

void *vrealloc(void *ptr, size_t sz)
{
	if (ptr) {
		vlt__alloc_node_t *old_node = (vlt__alloc_node_t*)ptr - 1;
		vlt__record_free(old_node->sz);
		if (sz) {
			vlt__alloc_node_t *node = realloc(old_node, sizeof(vlt__alloc_node_t) + sz);
			vlt__record_alloc(sz);
			node->sz = sz;
			node->generation = g_vlt_allocs.generation;
			if (node != old_node) {
				if (node->prev)
					node->prev->next = node;
				if (node->next)
					node->next->prev = node;
				if (g_vlt_allocs.head == old_node)
					g_vlt_allocs.head = node;
				if (g_vlt_allocs.tail == old_node)
					g_vlt_allocs.tail = node;
			}
			return node + 1;
		} else {
			vfree(ptr);
			return NULL;
		}
	} else if (sz) {
		return vmalloc(sz);
	} else {
		return NULL;
	}
}

void vfree(void *ptr)
{
	if (ptr) {
		vlt__alloc_node_t *node = (vlt__alloc_node_t*)ptr - 1;
		vlt__record_free(node->sz);
		if (node->prev)
			node->prev->next = node->next;
		if (node->next)
			node->next->prev = node->prev;
		if (g_vlt_allocs.head == node)
			g_vlt_allocs.head = node->next;
		if (g_vlt_allocs.tail == node)
			g_vlt_allocs.tail = node->prev;
		free(node);
	}
}

#undef malloc
#undef calloc
#undef realloc
#undef free
#define malloc(sz)         vmalloc(sz)
#define calloc(nmemb, sz)  vcalloc(nmemb, sz)
#define realloc(ptr, sz)   vrealloc(ptr, sz)
#define free(ptr)          vfree(ptr)

void vlt_mem_generation_advance()
{
	++g_vlt_allocs.generation;
	assert(g_vlt_allocs.generation != 0);
}

void vlt_mem_dump()
{
	if (g_vlt_allocs.current_bytes != 0)
		log_warn("%lu bytes still allocated!", g_vlt_allocs.current_bytes);

	vlt__alloc_node_t *node = g_vlt_allocs.head;
	while (node) {
		log_warn("%p: %10lu bytes @ gen %10lu still active!", node + 1, node->sz,
		         node->generation);
		node = node->next;
	}

	log_info("peak:  %10lu bytes allocated", g_vlt_allocs.peak_bytes);
	log_info("total: %10lu bytes allocated in %lu chunks",
	         g_vlt_allocs.total_bytes, g_vlt_allocs.total_chunks);
}

void vlt_mem_dump_current(vlt_mem_dump_callback_t cb, void *udata)
{
	vlt__alloc_node_t *node = g_vlt_allocs.head;
	while (node) {
		cb(node + 1, node->generation, udata);
		node = node->next;
	}
}

void vlt_mem_dump_file(void *ptr, u32 gen, void *udata)
{
	fprintf((FILE*)udata, "%p: %u", ptr, gen);
}


#endif

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

typedef struct log_stream
{
	logger_t logger;
	log_level_t level;
	void *udata;
} log_stream_t;

log_stream_t g_log_streams[LOG_STREAM_CAP];
u32 g_log_stream_cnt = 0;

void log_add_stream(log_level_t level, logger_t logger, void *udata)
{
	assert(g_log_stream_cnt < LOG_STREAM_CAP);
	g_log_streams[g_log_stream_cnt].logger = logger;
	g_log_streams[g_log_stream_cnt].level = level;
	g_log_streams[g_log_stream_cnt].udata = udata;
	++g_log_stream_cnt;
}

void log_remove_stream(logger_t logger, void *udata)
{
	for (u32 i = 0; i < g_log_stream_cnt; ++i) {
		if (logger == g_log_streams[i].logger && udata == g_log_streams[i].udata) {
			g_log_streams[i] = g_log_streams[g_log_stream_cnt-1];
			--g_log_stream_cnt;
			return;
		}
	}
	assert(false);
}

void log_level_write(log_level_t level, const char *format, ...)
{
	for (u32 i = 0; i < g_log_stream_cnt; ++i) {
		if (level & g_log_streams[i].level) {
			va_list ap;
			va_start(ap, format);
			g_log_streams[i].logger(g_log_streams[i].udata, format, ap);
			va_end(ap);
		}
	}
}

void file_logger(void *udata, const char *format, va_list ap)
{
	FILE *fp = udata;
	vfprintf(fp, format, ap);
	fputc('\n', fp);
	fflush(fp);
}

#undef CORE_IMPLEMENTATION
#endif // CORE_IMPLEMENTATION
