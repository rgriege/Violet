#ifndef VIOLET_CORE_H
#define VIOLET_CORE_H

#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <malloc.h>
#endif

/* Utility macros */

#define optional(type) type*

#define countof(x) (sizeof(x) / sizeof((x)[0]))

/* Buffer To Pointer + Size
 * Buffer To Pointer + Count */
#define B2PS(x) (x), sizeof(x)
#define B2PC(x) (x), countof(x)

#ifndef offsetof
#define offsetof(s, m) ((size_t)&(((s*)(NULL))->m))
#endif

#define polarity(x) (((x) > 0) - ((x) < 0))

#if !defined(__cplusplus) && !defined(static_assert)
#if __GNUC__ >= 5
#define static_assert _Static_assert
#else
#define static_assert(cnd, msg) typedef int static_assertion[(cnd) ? 1 : -1]
#endif
#endif

#define UNUSED(x) ((void)(x))
#define NOOP UNUSED(0)

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)

#define LOCATION __FILE__ ":" STRINGIFY(__LINE__)

#ifndef NDEBUG
#define check(x) assert(x)
#else
#define check(x) x
#endif

#ifndef WIN32
#define thread_local __thread
#else
#define thread_local __declspec(thread)
#endif

#if defined(__GNUC__) && (__GNUC__ >= 4)
#define CHECK_RETURN __attribute__ ((warn_unused_result))
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
#define CHECK_RETURN _Check_return_
#else
#define CHECK_RETURN
#endif

/* Types */

typedef bool b8;
typedef uint8_t u8;
typedef int8_t s8;
typedef int16_t b16;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t b32;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
typedef float r32;
typedef double r64;

typedef union {
	u8  bytes[16];
	u16 words[8];
	u32 dwords[4];
	u64 qwords[2];
} u128;

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

static_assert(sizeof(r32) == 4, "invalid floating point size");
static_assert(sizeof(r64) == 8, "invalid double size");

/* Initialization */

typedef enum vlt_thread_type
{
	VLT_THREAD_MAIN,
	VLT_THREAD_OTHER,
} vlt_thread_type_e;

void vlt_init(vlt_thread_type_e thread_type);
b32  vlt_is_init(void);
void vlt_destroy(vlt_thread_type_e thread_type);

/* Error handling */

void fatal(const char *msg);


/* Memory allocation */

/* Provide a compile option VLT_TRACK_MEMORY to override the default alloc.
 * This enables memory tracking without any add'l usage code and
 * allows easier memory tracking for imported libraries.
 *
 * Defining VLT_TRACK_MEMORY_VERBOSE will log ALL allocations,
 * which is occasionally useful but is quite verbose.
 *
 * Defining PGB_ANALYZE will help track down inefficient use
 * of the temporary allocator.
 * */

#ifdef VLT_TRACK_MEMORY
#define MEMCALL_LOCATION , LOCATION
#define MEMCALL_ARGS , const char *loc
#define MEMCALL_VARS , loc
#else
#define MEMCALL_LOCATION
#define MEMCALL_ARGS
#define MEMCALL_VARS
#endif

typedef struct allocator allocator_t;
typedef struct allocator
{
	void* (*malloc_)(size_t size, allocator_t *a  MEMCALL_ARGS);
	void* (*calloc_)(size_t nmemb, size_t size, allocator_t *a  MEMCALL_ARGS);
	void* (*realloc_)(void *ptr, size_t size, allocator_t *a  MEMCALL_ARGS);
	void  (*free_)(void *ptr, allocator_t *a  MEMCALL_ARGS);
	void *udata;
} allocator_t;

#define allocator_create(name, udata_) \
	(allocator_t){ \
		.malloc_  = name##_malloc, \
		.calloc_  = name##_calloc, \
		.realloc_ = name##_realloc, \
		.free_    = name##_free, \
		.udata    = udata_, \
	}

#define amalloc(sz, a)          (a)->malloc_(sz, a  MEMCALL_LOCATION)
#define acalloc(nmemb, sz, a)   (a)->calloc_(nmemb, sz, a  MEMCALL_LOCATION)
#define arealloc(p, sz, a)      (a)->realloc_(p, sz, a  MEMCALL_LOCATION)
#define afree(p, a)             (a)->free_(p, a  MEMCALL_LOCATION)

/* Default global allocator */
extern allocator_t *g_allocator;

void *default_malloc(size_t size, allocator_t *a  MEMCALL_ARGS);
void *default_calloc(size_t nmemb, size_t size, allocator_t *a  MEMCALL_ARGS);
void *default_realloc(void *ptr, size_t size, allocator_t *a  MEMCALL_ARGS);
void  default_free(void *ptr, allocator_t *a  MEMCALL_ARGS);

/* Temporary memory allocator */
extern thread_local allocator_t *g_temp_allocator;

#define PGB_MALLOC std_malloc
#define PGB_FREE   std_free
#ifdef VLT_TRACK_MEMORY
#define PGB_TRACK_MEMORY
#endif
#define PGB_LOG log_warn
#define PGB_LOG_ALLOC log_alloc
#define PGB_LOG_REALLOC log_realloc
#include "violet/pgb.h"

typedef pgb_watermark_t temp_memory_mark_t;
#define     temp_memory_save(alloc)    pgb_save((alloc)->udata)
#define     temp_memory_restore(mark)  pgb_restore(mark)
allocator_t temp_memory_fork_(pgb_t *pgb);
#define     temp_memory_fork()         temp_memory_fork_(&(pgb_t){0})
void        temp_memory_merge(allocator_t *fork);
void        temp_memory_merge_all(void);

void *temp_malloc(size_t size, allocator_t *a  MEMCALL_ARGS);
void *temp_calloc(size_t nmemb, size_t size, allocator_t *a  MEMCALL_ARGS);
void *temp_realloc(void *ptr, size_t size, allocator_t *a  MEMCALL_ARGS);
void  temp_free(void *ptr, allocator_t *a  MEMCALL_ARGS);

/* Tracking allocator */
typedef struct alloc_node
{
	struct alloc_node *prev, *next;
	size_t sz, generation;
	const char *location;
} alloc_node_t;

typedef struct alloc_tracker
{
	alloc_node_t *head, *tail;
	size_t generation;
	size_t current_bytes, peak_bytes, total_bytes;
	size_t total_chunks, total_allocs;
} alloc_tracker_t;

void alloc_tracker_advance_gen(alloc_tracker_t *tracker);
void alloc_tracker_log_usage(alloc_tracker_t *tracker, b32 warn_active_allocations);
void alloc_tracker_log_current_gen(alloc_tracker_t *tracker, size_t gen);

void *tracked_malloc(size_t size, allocator_t *a  MEMCALL_ARGS);
void *tracked_calloc(size_t nmemb, size_t size, allocator_t *a  MEMCALL_ARGS);
void *tracked_realloc(void *ptr, size_t size, allocator_t *a  MEMCALL_ARGS);
void  tracked_free(void *ptr, allocator_t *a  MEMCALL_ARGS);

void vlt_mem_advance_gen(void);
void vlt_mem_stats(size_t *global_bytes_used, size_t *global_bytes_peak,
                   size_t *global_bytes_total, size_t *global_alloc_count,
                   size_t *temp_bytes_used, size_t *temp_bytes_peak);
void vlt_mem_log_usage(void);

#ifdef VLT_TRACK_MEMORY

/* ensure access to stdlib functions before overriding them */
static inline void *std_malloc(size_t sz) { return malloc(sz); }
static inline void *std_calloc(size_t nmemb, size_t sz) { return calloc(nmemb, sz); }
static inline void *std_realloc(void *ptr, size_t sz) { return realloc(ptr, sz); }
static inline void  std_free(void *ptr) { free(ptr); }

#define malloc(sz)         amalloc(sz, g_allocator)
#define calloc(nmemb, sz)  acalloc(nmemb, sz, g_allocator)
#define realloc(ptr, sz)   arealloc(ptr, sz, g_allocator)
#define free(ptr)          afree(ptr, g_allocator)

#else

#define std_malloc(sz) malloc(sz)
#define std_calloc(nmemb, sz) calloc(nmemb, sz)
#define std_realloc(ptr, sz) realloc(ptr, sz)
#define std_free(ptr) free(ptr)

#endif // VLT_TRACK_MEMORY

/* Hash */

u32 hash_compute(const char *str);
u32 hash_compute_seeded(const char *str, u32 seed);
u32 hashn_compute(const char *str, u32 n);
u32 hashn_compute_seeded(const char *str, u32 n, u32 seed);

/* Utility */

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#define max(x, y) (((x) < (y)) ? (y) : (x))
#define min(x, y) (((x) > (y)) ? (y) : (x))

#define clamp(lo, val, hi) max(lo, min(hi, val))

#define memswp(a, b, type) \
	do { type tmp = a; a = b; b = tmp; } while(0)

#define memclr(x) memset(&(x), 0, sizeof(x))
#define arrclr(x) memset( (x), 0, sizeof(x))

#define arrcmp(x, y) memcmp((x), (y), sizeof(x))

#define arrcpy(x, y) memcpy((x), (y), sizeof(x))
#define arrcpyn(x, y, n) memcpy((x), (y), n * sizeof(*x))

void swap(void *lhs, void *rhs, size_t size);

void reverse(void *data, size_t size, size_t count);
#define reverse_buf(buf) reverse(buf, sizeof((buf)[0]), countof(buf))

void ssort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *));
void isort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *));
int  sort_s32_asc(const void *lhs, const void *rhs);
int  sort_s32_desc(const void *lhs, const void *rhs);
int  sort_u32_asc(const void *lhs, const void *rhs);
int  sort_u32_desc(const void *lhs, const void *rhs);
int  sort_r32_asc(const void *lhs, const void *rhs);
int  sort_r32_desc(const void *lhs, const void *rhs);

int find_u32(const void *lhs, const void *rhs);
int find_s32(const void *lhs, const void *rhs);

void buf_insert_(void *p, size_t idx, size_t n, size_t cap, size_t size);
#define buf_insert_n(p, idx, val, n, cap) \
	do { \
		const size_t buf_insert_idx_ = idx; \
		buf_insert_(p, buf_insert_idx_, n, cap, sizeof(*(p))); \
		memcpy(p+buf_insert_idx_, val, sizeof(*(p))*n); \
	} while (0)
#define buf_insert(p, idx, val, cap) buf_insert_n(p, idx, &val, 1, cap)

void buf_remove_(void *p, size_t idx, size_t n, size_t cap, size_t size);
#define buf_remove(p, idx, cap)      buf_remove_(p, idx, 1, cap, sizeof(*p))
#define buf_remove_n(p, idx, n, cap) buf_remove_(p, idx, n, cap, sizeof(*p))

#define buf_rotate_left(p, cap, type) \
	do { \
		const type buf_shift__p0 = p[0]; \
		buf_remove(p, 0, cap); \
		p[cap-1] = buf_shift__p0; \
	} while (0)

typedef u128 uuid;
#define UUID_BUF_SZ 37

uuid uuid_null(void);
uuid uuid_create(void);
b32  uuid_from_str(const char *in, uuid *out);
void uuid_to_str(uuid in, char out[UUID_BUF_SZ]);
b32  uuid_equal(uuid lhs, uuid rhs);
b32  uuid_is_valid(uuid id);
int  find_uuid(const void *lhs, const void *rhs);

/* Time */

/* This is only for intervals - a timepoint does not represent the unix epoch */
timepoint_t timepoint_create(void);
u32         timepoint_diff_seconds(timepoint_t start, timepoint_t end);
u32         timepoint_diff_milli(timepoint_t start, timepoint_t end);
u32         timepoint_diff_micro(timepoint_t start, timepoint_t end);
u32         timepoint_diff_nano(timepoint_t start, timepoint_t end);
/* shortcuts for common timepoint_diff(start, timepoint_create()) pattern */
u32         timepoint_expired_seconds(timepoint_t start);
u32         timepoint_expired_milli(timepoint_t start);
u32         timepoint_expired_micro(timepoint_t start);
u32         timepoint_expired_nano(timepoint_t start);

s64         time_seconds_since_epoch(void);
s64         time_milliseconds_since_epoch(void);
void        time_sleep_milli(u32 milli);

/* Log */

typedef enum log_level
{
	LOG_DEBUG   = 0x1,
	LOG_INFO    = 0x2,
	LOG_WARNING = 0x4,
	LOG_ERROR   = 0x8,
	LOG_FATAL   = 0x10,
	LOG_ASSERT  = 0x20,
	LOG_ALL     = (LOG_DEBUG | LOG_INFO | LOG_WARNING | LOG_ERROR | LOG_FATAL | LOG_ASSERT),
	LOG_STDOUT  = (LOG_DEBUG | LOG_INFO),
	LOG_STDERR  = (LOG_WARNING | LOG_ERROR | LOG_FATAL | LOG_ASSERT),
} log_level_e;

typedef void(*logger_t)(void *udata, log_level_e level,
                        const char *format, va_list ap);

#define LOG_STREAM_CAP 3

void log_add_stream(log_level_e level, logger_t logger, void *udata);
void log_remove_stream(logger_t logger, const void *udata);
void log_set_stream_level(logger_t logger, const void *udata, log_level_e level);
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
void log_level_write(log_level_e level, const char *format, ...);
void log_level_writev(log_level_e level, const char *format, va_list ap);
#define log_debug(fmt, ...) log_level_write(LOG_DEBUG,   fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)  log_level_write(LOG_INFO,    fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...)  log_level_write(LOG_WARNING, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...) log_level_write(LOG_ERROR,   fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...) log_level_write(LOG_FATAL,   fmt, ##__VA_ARGS__)
#define log_assert(fmt, ...) log_level_write(LOG_ASSERT, fmt, ##__VA_ARGS__)
#define log_write(fmt, ...) log_level_write(LOG_INFO,    fmt, ##__VA_ARGS__)

#define LOG_ASSERT_ONCE_(has_logged_once, fmt, ...) do { \
		static b32 has_logged_once = false; \
		if (!has_logged_once) { \
			log_assert(LOCATION " " fmt, ##__VA_ARGS__); \
			has_logged_once = true; \
		} \
	} while (0)
#define ASSERT_FALSE_AND_LOG(fmt, ...) do { \
	assert(false); \
	LOG_ASSERT_ONCE_(CONCAT(__has_logged_once, __COUNTER__), fmt, ##__VA_ARGS__); \
} while (0)

void file_logger(void *udata, log_level_e level, const char *format, va_list ap);
#if defined(_WIN32)
void msvc_debug_logger(void *udata, log_level_e level, const char *format, va_list ap);
#endif

#ifdef VIOLET_MAIN
int app_entry(int argc, char *const argv[]);
#if defined(_WIN32) && defined(UNICODE)
const char *os_imstr_to_utf8(const wchar_t *str);
int wmain(int argc, wchar_t *const wargv[], wchar_t *const envp[])
#else
int main(int argc, char *const argv[])
#endif
{
	int ret;
	vlt_init(VLT_THREAD_MAIN);
#if defined(_WIN32) && defined(UNICODE)
	char **argv = malloc(argc * sizeof(char*));
	for (int i = 0; i < argc; ++i) {
		const char *arg = os_imstr_to_utf8(wargv[i]);
		argv[i] = malloc(strlen(arg)+1);
		strcpy(argv[i], arg);
	}
#endif
	ret = app_entry(argc, argv);
#if defined(_WIN32) && defined(UNICODE)
	for (int i = 0; i < argc; ++i)
		free(argv[i]);
	free(argv);
#endif
#ifndef __EMSCRIPTEN__
	vlt_destroy(VLT_THREAD_MAIN);
#endif
	return ret;
}

#define main app_entry
#endif // VIOLET_MAIN

#endif // VIOLET_CORE_H


/* Implementation */

#ifdef CORE_IMPLEMENTATION

/* Error handling */

void fatal(const char *msg)
{
	log_fatal(msg);
	assert(false);
	exit(1);
}


/* Memory allocation */

#ifdef VLT_TRACK_MEMORY

#include <SDL2/SDL_thread.h>

typedef struct global_alloc_tracker
{
	SDL_mutex *mutex;
	alloc_tracker_t tracker;
} global_alloc_tracker_t;

void *global_tracked_malloc(size_t size, allocator_t *a  MEMCALL_ARGS)
{
	global_alloc_tracker_t *global_tracker = a->udata;
	allocator_t a_ = allocator_create(tracked, &global_tracker->tracker);
	void *p;
	SDL_LockMutex(global_tracker->mutex);
	p = tracked_malloc(size, &a_  MEMCALL_VARS);
	SDL_UnlockMutex(global_tracker->mutex);
	return p;
}

void *global_tracked_calloc(size_t nmemb, size_t size, allocator_t *a  MEMCALL_ARGS)
{
	global_alloc_tracker_t *global_tracker = a->udata;
	allocator_t a_ = allocator_create(tracked, &global_tracker->tracker);
	void *p;
	SDL_LockMutex(global_tracker->mutex);
	p = tracked_calloc(nmemb, size, &a_  MEMCALL_VARS);
	SDL_UnlockMutex(global_tracker->mutex);
	return p;
}

void *global_tracked_realloc(void *ptr, size_t size, allocator_t *a  MEMCALL_ARGS)
{
	global_alloc_tracker_t *global_tracker = a->udata;
	allocator_t a_ = allocator_create(tracked, &global_tracker->tracker);
	void *p;
	SDL_LockMutex(global_tracker->mutex);
	p = tracked_realloc(ptr, size, &a_  MEMCALL_VARS);
	SDL_UnlockMutex(global_tracker->mutex);
	return p;
}

void global_tracked_free(void *ptr, allocator_t *a  MEMCALL_ARGS)
{
	global_alloc_tracker_t *global_tracker = a->udata;
	allocator_t a_ = allocator_create(tracked, &global_tracker->tracker);
	SDL_LockMutex(global_tracker->mutex);
	tracked_free(ptr, &a_  MEMCALL_VARS);
	SDL_UnlockMutex(global_tracker->mutex);
}


allocator_t *g_allocator = &allocator_create(global_tracked, &(global_alloc_tracker_t){0});
#else
allocator_t *g_allocator = &allocator_create(default, NULL);
#endif

static
void log_alloc(const char *prefix, size_t sz  MEMCALL_ARGS)
{
#ifdef VLT_TRACK_MEMORY_VERBOSE
	log_debug("%s_alloc: %lu @ %s", prefix, sz, loc);
#endif
}

static
void log_realloc(const char *prefix, size_t sz  MEMCALL_ARGS)
{
#ifdef VLT_TRACK_MEMORY_VERBOSE
	log_debug("%s_realloc: %lu @ %s", prefix, sz, loc);
#endif
}

#define PGB_IMPLEMENTATION
#include "violet/pgb.h"

thread_local pgb_t g_temp_allocator_pgb    = {0};
thread_local allocator_t g_temp_allocator_ = {0};
thread_local allocator_t *g_temp_allocator = NULL;

thread_local pgb_heap_t g_temp_memory_heap = { .first_page = NULL };

allocator_t temp_memory_fork_(pgb_t *pgb)
{
	allocator_t allocator = allocator_create(temp, pgb);
	pgb_init(allocator.udata, &g_temp_memory_heap);
	return allocator;
}

void temp_memory_merge(allocator_t *fork)
{
	if (fork == g_temp_allocator) {
		ASSERT_FALSE_AND_LOG("invalid memory merge");
		return;
	}
	pgb_destroy(fork->udata);
}

void temp_memory_merge_all(void)
{
	pgb_heap_t *heap = &g_temp_memory_heap;
	for (pgb_page_t *page = heap->gfirst_page; page; page = page->gnext)
		if (   !pgb_has_page(g_temp_allocator->udata, page)
		    && !pgb_heap_has_page(heap, page))
			pgb_heap_return_page(heap, page);
}


/* Default allocator */

void *default_malloc(size_t size, allocator_t *a  MEMCALL_ARGS)
{
	return std_malloc(size);
}

void *default_calloc(size_t nmemb, size_t size, allocator_t *a  MEMCALL_ARGS)
{
	return std_calloc(nmemb, size);
}

void *default_realloc(void *ptr, size_t size, allocator_t *a  MEMCALL_ARGS)
{
	return std_realloc(ptr, size);
}

void default_free(void *ptr, allocator_t *a  MEMCALL_ARGS)
{
	std_free(ptr);
}


/* Temporary allocator */

void *temp_malloc(size_t size, allocator_t *a  MEMCALL_ARGS)
{
	return pgb_malloc(size, a->udata  MEMCALL_VARS);
}

void *temp_calloc(size_t nmemb, size_t size, allocator_t *a  MEMCALL_ARGS)
{
	return pgb_calloc(nmemb, size, a->udata  MEMCALL_VARS);
}

void *temp_realloc(void *ptr, size_t size, allocator_t *a  MEMCALL_ARGS)
{
	return pgb_realloc(ptr, size, a->udata  MEMCALL_VARS);
}

void temp_free(void *ptr, allocator_t *a  MEMCALL_ARGS)
{
	pgb_free(ptr, a->udata  MEMCALL_VARS);
}


/* Tracking allocator */

static
void alloc_tracker__record_alloc_(alloc_tracker_t *tracker, size_t sz)
{
	tracker->current_bytes += sz;
	if (tracker->peak_bytes < tracker->current_bytes)
		tracker->peak_bytes = tracker->current_bytes;
	tracker->total_bytes += sz;
	++tracker->total_allocs;
}

static
void alloc_tracker__record_alloc(alloc_tracker_t *tracker, size_t sz)
{
	alloc_tracker__record_alloc_(tracker, sz);
	++tracker->total_chunks;
}

static
void alloc_tracker__record_realloc(alloc_tracker_t *tracker, size_t sz)
{
	alloc_tracker__record_alloc_(tracker, sz);
}

static
void alloc_tracker__record_free(alloc_tracker_t *tracker, size_t sz)
{
	if (sz > tracker->current_bytes) {
		ASSERT_FALSE_AND_LOG("attempting to free more than allocated");
		// Could argue this should free max legal amount, but erring on the side
		// of it being an invalid operation we just ignore.
		return;
	}
	tracker->current_bytes -= sz;
}

static
void alloc_tracker__append_node(alloc_tracker_t *tracker,
                                alloc_node_t *node, size_t sz  MEMCALL_ARGS)
{
	node->sz = sz;
	node->generation = tracker->generation;
	node->next = NULL;
#ifdef VLT_TRACK_MEMORY
	node->location = loc;
#else
	node->location = "(unknown)";
#endif
	alloc_tracker__record_alloc(tracker, sz);
	if (tracker->tail) {
		tracker->tail->next = node;
		node->prev = tracker->tail;
		tracker->tail = node;
	} else {
		tracker->head = node;
		tracker->tail = node;
		node->prev = NULL;
	}
}

void alloc_tracker_advance_gen(alloc_tracker_t *tracker)
{
	++tracker->generation;
	if (tracker->generation == 0)
		log_warn("generation wrap-around");
}

void alloc_tracker_log_usage(alloc_tracker_t *tracker, b32 warn_active_allocations)
{
	log_info("***HEAP***");

	if (warn_active_allocations) {
		alloc_node_t *node = tracker->head;
		while (node) {
			log_warn("%p: %6lu bytes still active from %s @ gen %lu!",
			         node + 1, node->sz, node->location, node->generation);
			node = node->next;
		}

		if (tracker->current_bytes != 0)
			log_warn("exit:  %10lu bytes still allocated!", tracker->current_bytes);
	}

	log_info("peak:  %10lu bytes", tracker->peak_bytes);
	log_info("total: %10lu bytes in %lu chunks, %lu (re)allocs",
	         tracker->total_bytes, tracker->total_chunks, tracker->total_allocs);
}

void alloc_tracker_log_current_gen(alloc_tracker_t *tracker, size_t gen)
{
	alloc_node_t *node = tracker->head;
	log_info("memory generation %lu:", gen);
 	while (node) {
		if (node->generation == gen)
			log_info("%p: %10lu bytes from %s", node + 1, node->sz, node->location);
 		node = node->next;
 	}
}

void *tracked_malloc(size_t sz, allocator_t *a  MEMCALL_ARGS)
{
	alloc_node_t *node = std_malloc(sizeof(alloc_node_t) + sz);
	alloc_tracker__append_node(a->udata, node, sz  MEMCALL_VARS);
	log_alloc("std", sz  MEMCALL_VARS);
	return node + 1;
}

void *tracked_calloc(size_t nmemb, size_t sz, allocator_t *a  MEMCALL_ARGS)
{
	alloc_node_t *node = std_calloc(1, sizeof(alloc_node_t) + nmemb * sz);
	alloc_tracker__append_node(a->udata, node, nmemb * sz  MEMCALL_VARS);
	log_alloc("std", nmemb * sz  MEMCALL_VARS);
	return node + 1;
}

void *tracked_realloc(void *ptr, size_t sz, allocator_t *a  MEMCALL_ARGS)
{
	alloc_tracker_t *tracker = a->udata;
	if (ptr) {
		alloc_node_t *old_node = (alloc_node_t*)ptr - 1;
		const size_t old_sz = old_node->sz;
		if (sz) {
			alloc_node_t *node = std_realloc(old_node, sizeof(alloc_node_t) + sz);
			if (node != old_node) {
				alloc_tracker__record_free(tracker, old_sz);
				alloc_tracker__record_alloc(tracker, sz);
			} else if (sz > old_sz) {
				alloc_tracker__record_realloc(tracker, sz - old_sz);
			} else if (sz < old_sz) {
				alloc_tracker__record_free(tracker, old_sz - sz);
			}
			log_realloc("std", sz  MEMCALL_VARS);
			node->sz = sz;
			node->generation = tracker->generation;
#ifdef VLT_TRACK_MEMORY
			node->location = loc;
#else
			node->location = "(unknown)";
#endif
			if (node != old_node) {
				if (node->prev)
					node->prev->next = node;
				if (node->next)
					node->next->prev = node;
				if (tracker->head == old_node)
					tracker->head = node;
				if (tracker->tail == old_node)
					tracker->tail = node;
			}
			return node + 1;
		} else {
			tracked_free(ptr, a  MEMCALL_VARS);
			return NULL;
		}
	} else if (sz) {
		return tracked_malloc(sz, a  MEMCALL_VARS);
	} else {
		return NULL;
	}
}

void tracked_free(void *ptr, allocator_t *a  MEMCALL_ARGS)
{
	if (ptr) {
		alloc_tracker_t *tracker = a->udata;
		alloc_node_t *node = (alloc_node_t*)ptr - 1;
		if (node->prev)
			node->prev->next = node->next;
		if (node->next)
			node->next->prev = node->prev;
		if (tracker->head == node)
			tracker->head = node->next;
		if (tracker->tail == node)
			tracker->tail = node->prev;
		alloc_tracker__record_free(a->udata, node->sz);
		std_free(node);
	}
}

void vlt_mem_advance_gen(void)
{
#ifdef VLT_TRACK_MEMORY
	global_alloc_tracker_t *global_tracker = g_allocator->udata;
	SDL_LockMutex(global_tracker->mutex);
	alloc_tracker_advance_gen(&global_tracker->tracker);
	SDL_UnlockMutex(global_tracker->mutex);
#endif
}

void vlt_mem_stats(size_t *global_bytes_used, size_t *global_bytes_peak,
                   size_t *global_bytes_total, size_t *global_alloc_count,
                   size_t *temp_bytes_used, size_t *temp_bytes_peak)
{
	size_t temp_pages_used, temp_pages_available;
#ifdef VLT_TRACK_MEMORY
	global_alloc_tracker_t *global_tracker = g_allocator->udata;
	SDL_LockMutex(global_tracker->mutex);
	*global_bytes_used  = global_tracker->tracker.current_bytes;
	*global_bytes_peak  = global_tracker->tracker.peak_bytes;
	*global_bytes_total = global_tracker->tracker.total_bytes;
	*global_alloc_count = global_tracker->tracker.total_allocs;
	SDL_UnlockMutex(global_tracker->mutex);
#else
	*global_bytes_used  = 0;
	*global_bytes_peak  = 0;
	*global_bytes_total = 0;
	*global_alloc_count = 0;
#endif
	pgb_stats(g_temp_allocator->udata, temp_bytes_used, &temp_pages_used, temp_bytes_peak, &temp_pages_available);
}

#ifdef __EMSCRIPTEN__
#include <unistd.h>
EM_JS(size_t, vlt_emscripten_get_total_memory, (), { return HEAP8.length; });
size_t vlt_emscripten_get_used_memory(void)
{
	struct mallinfo info = mallinfo();
	size_t dynamic_top = (size_t)sbrk(0);
	return dynamic_top + info.fordblks;
}
#endif

static
void vlt_mem_log_usage_(size_t temp_bytes_current, size_t temp_pages_current,
                        size_t temp_bytes_total, size_t temp_pages_total,
                        b32 warn_active_allocations)
{
	log_info("memory diagnostic:");
#ifdef VLT_TRACK_MEMORY
	{
		global_alloc_tracker_t *global_tracker = g_allocator->udata;
		SDL_LockMutex(global_tracker->mutex);
		alloc_tracker_log_usage(&global_tracker->tracker, warn_active_allocations);
		SDL_UnlockMutex(global_tracker->mutex);
	}
#endif
	log_info("***TEMP***");
	log_info("temp:");

	if (warn_active_allocations && temp_bytes_current != 0)
		log_warn("%6lu bytes still active in %lu pages!",
		         temp_bytes_current, temp_pages_current);

	log_info("current: %8lu bytes in %lu pages", temp_bytes_current, temp_pages_current);
	log_info("total:   %8lu bytes in %lu pages", temp_bytes_total, temp_pages_total);

#ifdef __EMSCRIPTEN__
	const size_t em_used_mem = vlt_emscripten_get_used_memory();
	const size_t em_total_mem = vlt_emscripten_get_total_memory();
	log_info("***EMSCRIPTEN***");
	log_info("heap:");
	log_info("used:  %10lu bytes", em_used_mem);
	log_info("total: %10lu bytes", em_total_mem);
#endif
}

void vlt_mem_log_usage(void)
{
	const pgb_t *pgb = g_temp_allocator->udata;
	size_t bytes_used, pages_used, bytes_total, pages_total;
	pgb_stats(pgb, &bytes_used, &pages_used, &bytes_total, &pages_total);
	vlt_mem_log_usage_(bytes_used, pages_used, bytes_total, pages_total, false);
}


/* Hash (djb2 by dan bernstein) */

u32 hash_compute(const char *str)
{
	return hash_compute_seeded(str, 5381);
}

u32 hash_compute_seeded(const char *str, u32 seed)
{
	u32 hash = seed;
	int c;
	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;
	return hash;
}

u32 hashn_compute(const char *str, u32 n)
{
	return hashn_compute_seeded(str, n, 5381);
}

u32 hashn_compute_seeded(const char *str, u32 n, u32 seed)
{
	u32 hash = seed;
	for (u32 i = 0; i < n; ++i)
		hash = ((hash << 5) + hash) + (int)str[i];
	return hash;
}

void swap(void *lhs_, void *rhs_, size_t size_)
{
	char *lhs = lhs_, *rhs = rhs_;
	size_t size = size_;
	char tmp[128];
	do {
		const size_t size_iter = min(size, sizeof(tmp));
		memcpy(tmp, lhs, size_iter);
		memcpy(lhs, rhs, size_iter);
		memcpy(rhs, tmp, size_iter);
		lhs += size_iter;
		rhs += size_iter;
		size -= size_iter;
	} while (size > 0);
}

void reverse(void *data_, size_t size, size_t count)
{
	char *data = data_;
	for (size_t i = 0, n = count/2; i < n; ++i)
		swap(data+i*size, data+(count-1-i)*size, size);
}

#include "violet/ssort.c"

void isort(void *base_, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *))
{
	char *base = base_;
	for (size_t i = 1; i < nmemb; ++i)
		for (size_t j = i; j > 0 && compar(base+(j-1)*size, base+j*size) > 0; --j)
			swap(base+(j-1)*size, base+j*size, size);
}

int sort_s32_asc(const void *lhs_, const void *rhs_)
{
	const s32 *lhs = lhs_, *rhs = rhs_;
	return *lhs - *rhs;
}

int sort_s32_desc(const void *lhs_, const void *rhs_)
{
	const s32 *lhs = lhs_, *rhs = rhs_;
	return *rhs - *lhs;
}

int sort_u32_asc(const void *lhs_, const void *rhs_)
{
	const u32 lhs = *(const u32*)lhs_;
	const u32 rhs = *(const u32*)rhs_;
	return lhs < rhs ? -1 : lhs > rhs ? 1 : 0;
}

int sort_u32_desc(const void *lhs_, const void *rhs_)
{
	const u32 lhs = *(const u32*)lhs_;
	const u32 rhs = *(const u32*)rhs_;
	return lhs < rhs ? 1 : lhs > rhs ? -1 : 0;
}

int sort_r32_asc(const void *lhs_, const void *rhs_)
{
	const r32 lhs = *(const r32*)lhs_;
	const r32 rhs = *(const r32*)rhs_;
	return (lhs > rhs) - (lhs < rhs);
}

int sort_r32_desc(const void *lhs_, const void *rhs_)
{
	const r32 lhs = *(const r32*)lhs_;
	const r32 rhs = *(const r32*)rhs_;
	return (rhs > lhs) - (rhs < lhs);
}

int find_u32(const void *lhs_, const void *rhs_)
{
	const u32 lhs = *(const u32*)lhs_;
	const u32 rhs = *(const u32*)rhs_;
	return lhs == rhs ? 0 : 1;
}

int find_s32(const void *lhs_, const void *rhs_)
{
	const s32 lhs = *(const u32*)lhs_;
	const s32 rhs = *(const u32*)rhs_;
	return lhs == rhs ? 0 : 1;
}

void buf_insert_(void *p_, size_t idx, size_t n, size_t cap, size_t size)
{
	if (idx + n > cap)
		ASSERT_FALSE_AND_LOG("attempting to insert beyond capacity");
	// No early return on assert since memmove statement takes capacity into account
	char *p = p_;
	memmove(p+(idx+n)*size, p+idx*size, (cap-n-idx)*size);
}

void buf_remove_(void *p_, size_t idx, size_t n, size_t cap, size_t size)
{
	if (idx + n > cap)
		ASSERT_FALSE_AND_LOG("attempting to remove beyond end of array");
	// No early return on assert since memmove statement takes capacity into account
	char *p = p_;
	memmove(p+idx*size, p+(idx+n)*size, (cap-n-idx)*size);
}

#ifdef _WIN32
#include <combaseapi.h>
uuid uuid_create(void)
{
	uuid id = {0};
	HRESULT result = CoCreateGuid((UUID*)&id);
	if (result != S_OK)
		ASSERT_FALSE_AND_LOG("HRESULT = %d", result);
	return id;
}

b32 os_string_from_utf8(wchar_t *dst, size_t dstlen, const char *src);
b32 uuid_from_str(const char *in, uuid *out)
{
	wchar_t win[39] = {0};
	win[0] = L'{';
	if (strlen(in) != 36)
		return false;
	if (!os_string_from_utf8(&win[1], 37, in)) {
		ASSERT_FALSE_AND_LOG("failed to convert uuid string to wide string");
		return false;
	}
	win[UUID_BUF_SZ] = L'}';
	return CLSIDFromString(win, (UUID*)out) == NOERROR;
}

b32 os_string_to_utf8(char *dst, size_t dstlen, const osstr_t src);
void uuid_to_str(uuid in, char out[UUID_BUF_SZ])
{
	wchar_t wout[39];
	if (StringFromGUID2((UUID*)&in, wout, 39) != 39) {
		strcpy(out, "00000000-0000-0000-0000-000000000000");
		ASSERT_FALSE_AND_LOG("failed to convert uuid to string");
		return;
	}
	wout[UUID_BUF_SZ] = 0;
	if (!os_string_to_utf8(out, 37, &wout[1])) {
		strcpy(out, "00000000-0000-0000-0000-000000000000");
		ASSERT_FALSE_AND_LOG("failed to convert wide string to uuid string");
		return;
	}
	for (int i = 0; i < 36; ++i)
		out[i] = tolower(out[i]);
}
#else
#include <uuid/uuid.h>
uuid uuid_create(void)
{
	uuid id = {0};
	uuid_generate(id.bytes);
	return id;
}

b32 uuid_from_str(const char *in, uuid *out)
{
	return uuid_parse((char*)in, out->bytes) == 0;
}

void uuid_to_str(uuid in, char out[UUID_BUF_SZ])
{
	uuid_unparse_lower(in.bytes, out);
}
#endif

uuid uuid_null(void)
{
	return (uuid){0};
}

b32 uuid_equal(uuid lhs, uuid rhs)
{
	return memcmp(&lhs, &rhs, sizeof(lhs)) == 0;
}

b32 uuid_is_valid(uuid id)
{
	return !uuid_equal(id, uuid_null());
}

int find_uuid(const void *lhs_, const void *rhs_)
{
	const uuid lhs = *(const uuid*)lhs_;
	const uuid rhs = *(const uuid*)rhs_;
	return uuid_equal(lhs, rhs) ? 0 : 1;
}

/* Time */

#ifndef _WIN32

timepoint_t timepoint_create(void)
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

	res.tv_sec = end.tv_sec - mod_start.tv_sec;
	res.tv_nsec = end.tv_nsec - mod_start.tv_nsec;
	/* C standard states that tv_nsec should be [0, 999,999,999] */
	if (res.tv_nsec >= 1000000000)
		ASSERT_FALSE_AND_LOG("tv_nsec = %d", res.tv_nsec);
	if (end.tv_sec < mod_start.tv_sec)
		ASSERT_FALSE_AND_LOG("end.tv_sec = %d, mod_start.tv_sec = %d", end.tv_sec, mod_start.tv_sec);

	return res;
}

u32 timepoint_diff_seconds(timepoint_t start, timepoint_t end)
{
	timepoint_t res = time__diff(start, end);
	return (u32)res.tv_sec;
}

u32 timepoint_diff_milli(timepoint_t start, timepoint_t end)
{
	timepoint_t res = time__diff(start, end);
	return (u32)(res.tv_sec * 1000 + res.tv_nsec / 1000000);
}

u32 timepoint_diff_micro(timepoint_t start, timepoint_t end)
{
	timepoint_t res = time__diff(start, end);
	return (u32)(res.tv_sec * 1000000 + res.tv_nsec / 1000);
}

u32 timepoint_diff_nano(timepoint_t start, timepoint_t end)
{
	timepoint_t res = time__diff(start, end);
	return (u32)(res.tv_sec * 1000000000 + res.tv_nsec);
}

s64 time_seconds_since_epoch(void)
{
	/* This is true for POSIX but isn't technically guaranted by C */
	return (s64)time(NULL);
}

s64 time_milliseconds_since_epoch(void)
{
	/* This is true for POSIX but isn't technically guaranted by C */
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);
	return (s64)spec.tv_sec * 1000LL + (s64)spec.tv_nsec / 1000000LL;
}

void time_sleep_milli(u32 milli)
{
	timepoint_t t = { .tv_nsec = milli * 1000000 };
	nanosleep(&t, NULL);
}

#else

timepoint_t timepoint_create(void)
{
	timepoint_t t;
	QueryPerformanceCounter(&t);
	return t;
}

u32 timepoint_diff_seconds(timepoint_t start, timepoint_t end)
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	return (u32)((end.QuadPart - start.QuadPart) / frequency.QuadPart);
}

u32 timepoint_diff_milli(timepoint_t start, timepoint_t end)
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	return (u32)((end.QuadPart - start.QuadPart) * 1000 / frequency.QuadPart);
}

u32 timepoint_diff_micro(timepoint_t start, timepoint_t end)
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	return (u32)((end.QuadPart - start.QuadPart) * 1000000 / frequency.QuadPart);
}

u32 timepoint_diff_nano(timepoint_t start, timepoint_t end)
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	return (u32)((end.QuadPart - start.QuadPart) * 1000000000 / frequency.QuadPart);
}

static
ULARGE_INTEGER time__since_epoch_as_ularge()
{
	FILETIME f;
	ULARGE_INTEGER u;
	GetSystemTimeAsFileTime(&f);
	u.u.LowPart  = f.dwLowDateTime;
	u.u.HighPart = f.dwHighDateTime;
	return u;
}

s64 time_seconds_since_epoch(void)
{
	ULARGE_INTEGER u = time__since_epoch_as_ularge();
	return (s64)(u.QuadPart / 10000000) - 11644473600;
}

s64 time_milliseconds_since_epoch(void)
{
	ULARGE_INTEGER u = time__since_epoch_as_ularge();
	return (s64)(u.QuadPart / 10000)- 11644473600000;
}

void time_sleep_milli(u32 milli)
{
	Sleep(milli);
}

#endif

u32 timepoint_expired_seconds(timepoint_t start)
{
	return timepoint_diff_seconds(start, timepoint_create());
}

u32 timepoint_expired_milli(timepoint_t start)
{
	return timepoint_diff_milli(start, timepoint_create());
}

u32 timepoint_expired_micro(timepoint_t start)
{
	return timepoint_diff_micro(start, timepoint_create());
}

u32 timepoint_expired_nano(timepoint_t start)
{
	return timepoint_diff_nano(start, timepoint_create());
}

/* Log */

typedef struct log_stream
{
	logger_t logger;
	log_level_e level;
	void *udata;
} log_stream_t;

log_stream_t g_log_streams[LOG_STREAM_CAP];
u32 g_log_stream_cnt = 0;

void log_add_stream(log_level_e level, logger_t logger, void *udata)
{
	if (g_log_stream_cnt >= LOG_STREAM_CAP) {
		ASSERT_FALSE_AND_LOG("attempting to add too many loggers");
		return;
	}
	g_log_streams[g_log_stream_cnt].logger = logger;
	g_log_streams[g_log_stream_cnt].level = level;
	g_log_streams[g_log_stream_cnt].udata = udata;
	++g_log_stream_cnt;
}

void log_remove_stream(logger_t logger, const void *udata)
{
	for (u32 i = 0; i < g_log_stream_cnt; ++i) {
		if (logger == g_log_streams[i].logger && udata == g_log_streams[i].udata) {
			g_log_streams[i] = g_log_streams[g_log_stream_cnt-1];
			--g_log_stream_cnt;
			return;
		}
	}
	ASSERT_FALSE_AND_LOG("targeting invalid logger or logger with mismatched udata");
}

void log_set_stream_level(logger_t logger, const void *udata, log_level_e level)
{
	for (u32 i = 0; i < g_log_stream_cnt; ++i) {
		if (logger == g_log_streams[i].logger && udata == g_log_streams[i].udata) {
			g_log_streams[i].level = level;
			return;
		}
	}
	ASSERT_FALSE_AND_LOG("targeting invalid logger or logger with mismatched udata");
}

void log_level_write(log_level_e level, const char *format, ...)
{
	for (u32 i = 0; i < g_log_stream_cnt; ++i) {
		if (level & g_log_streams[i].level) {
			va_list ap;
			va_start(ap, format);
			g_log_streams[i].logger(g_log_streams[i].udata, level, format, ap);
			va_end(ap);
		}
	}
}

void log_level_writev(log_level_e level, const char *format, va_list ap)
{
	for (u32 i = 0; i < g_log_stream_cnt; ++i) {
		if (level & g_log_streams[i].level) {
			va_list cpy;
			va_copy(cpy, ap);
			g_log_streams[i].logger(g_log_streams[i].udata, level, format, cpy);
			va_end(cpy);
		}
	}
}

void file_logger(void *udata, log_level_e level, const char *format, va_list ap)
{
	FILE *fp = udata;
	switch (level) {
	case LOG_DEBUG:
		fputs("[DEBUG] ", fp);
	break;
	case LOG_INFO:
		fputs("[INFO ] ", fp);
	break;
	case LOG_WARNING:
		fputs("[WARN ] ", fp);
	break;
	case LOG_ERROR:
		fputs("[ERROR] ", fp);
	break;
	case LOG_FATAL:
		fputs("[FATAL] ", fp);
	break;
	case LOG_ASSERT:
		fputs("[ASSERT] ", fp);
	break;
	default:
		fputs("[XXXXX] ", fp);
	break;
	}
	vfprintf(fp, format, ap);
	fputc('\n', fp);
	fflush(fp);
}

#if defined(_WIN32)
b32 os_string_from_utf8(wchar_t *dst, size_t dstlen, const char *src);
void msvc_debug_logger(void *udata, log_level_e level, const char *format, va_list ap)
{
	static thread_local wchar_t str_w[1024];
	static thread_local char str[1024];
	const int n = vsnprintf(B2PC(str), format, ap);
	if (   n > 0
	    && n + 2 <= countof(str)
	    && strcpy(&str[n], "\n")
	    && os_string_from_utf8(B2PC(str_w), str))
		OutputDebugStringW(str_w);
}
#endif

/* Runtime */

#if defined(_WIN32) && defined(DEBUG_HEAP)
#include <crtdbg.h>
#endif

void vlt_init(vlt_thread_type_e thread_type)
{
#ifdef VLT_TRACK_MEMORY
	if (thread_type == VLT_THREAD_MAIN) {
		global_alloc_tracker_t *global_tracker = g_allocator->udata;
		global_tracker->mutex = SDL_CreateMutex();
	}
#endif
	pgb_heap_init(&g_temp_memory_heap);
	g_temp_allocator_ = allocator_create(temp, &g_temp_allocator_pgb);
	g_temp_allocator  = &g_temp_allocator_;
	pgb_init(g_temp_allocator->udata, &g_temp_memory_heap);

#if defined(_WIN32) && defined(DEBUG_HEAP)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF);
#endif
}

b32 vlt_is_init(void)
{
	return g_temp_allocator != NULL;
}

void vlt_destroy(vlt_thread_type_e thread_type)
{
	pgb_t *pgb = g_temp_allocator->udata;
	size_t bytes_used, pages_used, bytes_total, pages_total;
	pgb_stats(pgb, &bytes_used, &pages_used, &bytes_total, &pages_total);
	pgb_destroy(pgb);
	pgb_heap_destroy(&g_temp_memory_heap);
	vlt_mem_log_usage_(bytes_used, pages_used, bytes_total, pages_total,
	                   thread_type == VLT_THREAD_MAIN);
	g_temp_allocator = NULL;
#ifdef VLT_TRACK_MEMORY
	if (thread_type == VLT_THREAD_MAIN) {
		global_alloc_tracker_t *global_tracker = g_allocator->udata;
		SDL_DestroyMutex(global_tracker->mutex);
	}
#endif
}

#undef CORE_IMPLEMENTATION
#endif // CORE_IMPLEMENTATION
