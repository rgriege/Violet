#ifndef VIOLET_OS_H
#define VIOLET_OS_H

#include "violet/core.h"

/* File system */

extern const char *g_file_path_separator;

typedef struct {
	const char *name;
	const char *pattern;
} file_dialog_filter_t;

b32  file_open_dialog(char *fname, u32 fname_sz, file_dialog_filter_t filter);
b32  file_save_dialog(char *fname, u32 fname_sz, file_dialog_filter_t filter);
b32  file_open_dialog_ex(char *fname, u32 fname_sz,
                         const file_dialog_filter_t filters[], u32 num_filters);
b32  file_save_dialog_ex(char *fname, u32 fname_sz,
                         const file_dialog_filter_t filters[], u32 num_filters);

b32  file_exists(const char *path);
b32  dir_exists(const char *path);
void path_append(char *lhs, const char *rhs);
void path_appendn(char *lhs, const char *rhs, u32 sz);
b32  mkpath(const char *path);
#ifdef VLT_USE_TINYDIR
b32  rmdir_f(const char *path);
#endif

const char *app_dir(void);
char *app_data_dir(const char *app_name, allocator_t *a);

char *imapppath(const char *path_relative_to_app);

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
#include "violet/win32.h"
#elif defined(__APPLE__)
#include "violet/macos.h"
#elif defined(LINUX)
#include "violet/linux.h"
#else
#error "Platform not supported!"
#endif

#endif // OS_IMPLEMENTAITON
