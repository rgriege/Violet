#ifndef VIOLET_OS_H
#define VIOLET_OS_H

#include "violet/core.h"

#if   defined(_WIN32)
#define PLATFORM_NAME "Win32"
#elif defined(__APPLE__)
#define PLATFORM_NAME "MacOS"
#elif defined(LINUX)
#define PLATFORM_NAME "Linux"
#elif defined(__EMSCRIPTEN__)
#define PLATFORM_NAME "Web"
#else
#error "Platform not supported!"
#endif

/* Strings */
#ifdef _WIN32
#define osstr_t wchar_t*
#else
#define osstr_t char*
#endif

b32    os_string_from_utf8(osstr_t dst, size_t dstlen, const char *src);
size_t os_string_from_utf8_size(const char *src);
b32    os_string_to_utf8(char *dst, size_t dstlen, const osstr_t src);
size_t os_string_to_utf8_size(const osstr_t src);

/* these immediate strings are independent from the main imstr() */
const osstr_t os_imstr_from_utf8(const char *str);
const char   *os_imstr_to_utf8(const osstr_t str);

/* File system */

#ifdef _WIN32
#define PATH_MAX MAX_PATH
#endif

extern const char *g_file_path_separator;

typedef struct {
	const char *name;
	const char *pattern;
} file_dialog_filter_t;

b32  file_open_dialog(char *fname, u32 fname_sz, const file_dialog_filter_t filters[], u32 num_filters);
b32  file_save_dialog(char *fname, u32 fname_sz, const file_dialog_filter_t filters[], u32 num_filters);

b32  file_exists(const char *path);
b32  dir_exists(const char *path);
void path_append(char *lhs, const char *rhs);
void path_appendn(char *lhs, const char *rhs, u32 sz);
b32  mkpath(const char *path);
#ifdef VLT_USE_TINYDIR
b32  rmdir_f(const char *path);
#endif

char *impathcat(char *imstr, const char *path);
char *impathcatprintf(char *imstr, const char *fmt, ...);
char *imappdir(void);
char *imapppath(const char *resource);
char *imresdir(void);
char *imrespath(const char *resource);
char *imlogdir(void);
char *imlogpath(const char *resource);
char *imcachedir(void);
char *imcachepath(const char *resource);
char *imdatadir(void);
char *imdatapath(const char *resource);
char *imuserdatadir(void);
char *imuserdatapath(const char *resource);

FILE  *file_open(const char *fname, const char *mode);
void  *file_read_all(const char *fname, const char *mode, size_t *sz, allocator_t *a);

/* Dynamic library */

#ifndef VIOLET_NO_LIB
#ifdef _WIN32
typedef HMODULE lib_handle;
#else
typedef void* lib_handle;
#endif

lib_handle  lib_load(const char *filename);
void       *lib_func(lib_handle hnd, const char *name);
b32         lib_close(lib_handle hnd);
const char *lib_err(void);
#endif // VIOLET_NO_LIB

/* IO */

size_t vgetdelim(char **lineptr, size_t *n, int delim, FILE *stream, allocator_t *a);
size_t vgetline(char **lineptr, size_t *n, FILE *stream, allocator_t *a);

/* Other applications */

void exec(char *const argv[]);
int  run(const char *command);
b32  open_file_external(const char *filename);

/* System */

b32 cpu_supports_sse41(void);

#endif


/* Implementation */

#ifdef OS_IMPLEMENTATION
#undef OS_IMPLEMENTATION

#ifdef VLT_USE_TINYDIR
#if defined(__GNUC__) && (__GNUC__ >= 8)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wrestrict"
#endif
#define TINYDIR_IMPLEMENTATION
#include <tinydir/tinydir.h>
#if defined(__GNUC__) && (__GNUC__ >= 8)
#pragma GCC diagnostic pop
#endif
#endif

#if   defined(_WIN32)
#include "violet/windows.h"
#elif defined(__APPLE__)
#include "violet/macos.h"
#elif defined(LINUX)
#include "violet/linux.h"
#elif defined(__EMSCRIPTEN__)
#include "violet/emscripten.h"
#else
#error "Platform not supported!"
#endif

#endif // OS_IMPLEMENTAITON
