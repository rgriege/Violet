#ifndef VIOLET_OS_H
#define VIOLET_OS_H

#include "violet/core.h"

#if   defined(_WIN32)
#define PLATFORM_NAME "Win32"
#elif defined(__APPLE__)
#define PLATFORM_NAME "MacOS"
#elif defined(LINUX)
#define PLATFORM_NAME "Linux"
#else
#error "Platform not supported!"
#endif

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

void *file_read_all(const char *fname, const char *mode, allocator_t *a);

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
const char *lib_err();
#endif // VIOLET_NO_LIB

/* IO */

size_t vgetdelim(char **lineptr, size_t *n, int delim, FILE *stream, allocator_t *a);
size_t vgetline(char **lineptr, size_t *n, FILE *stream, allocator_t *a);

/* Other applications */

void exec(char *const argv[]);
int  run(const char *command);
b32  open_file_external(const char *filename);

#endif


/* Implementation */

#ifdef OS_IMPLEMENTATION
#undef OS_IMPLEMENTATION

#ifdef VLT_USE_TINYDIR
#define TINYDIR_IMPLEMENTATION
#include <tinydir/tinydir.h>
#endif

#if   defined(_WIN32)
#include "violet/windows.h"
#elif defined(__APPLE__)
#include "violet/macos.h"
#elif defined(LINUX)
#include "violet/linux.h"
#else
#error "Platform not supported!"
#endif

#endif // OS_IMPLEMENTAITON
