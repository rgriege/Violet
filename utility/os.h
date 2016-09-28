#ifndef VIOLET_OS_H
#define VIOLET_OS_H

#include "violet/core/types.h"

b8 vlt_file_open_dialog(char *filename, u32 n, const char *ext);
b8 vlt_file_save_dialog(char *filename, u32 n, const char *ext);

void path_append(char *lhs, const char *rhs);
b8 mkpath(const char *path);
b8 app_data_dir(char *path, u32 n);

#ifdef _WIN32
typedef HMODULE lib_handle;
#else
typedef void* lib_handle;
#endif

lib_handle lib_load(const char *filename);
void *lib_func(lib_handle hnd, const char *name);
b8 lib_close(lib_handle hnd);
const char *lib_err();

#endif
