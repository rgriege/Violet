#ifndef VIOLET_OS_H
#define VIOLET_OS_H

#include "violet/core/types.h"

b8 file_open_dialog(char *fname, u32 n, const char *ext);
b8 file_save_dialog(char *fname, u32 n, const char *ext);
b8 file_open_dialog_ex(char *fname, u32 n, const char *ext[], u32 n_ext);
b8 file_save_dialog_ex(char *fname, u32 n, const char *ext[], u32 n_ext);

b8 file_exists(const char *path);
b8 dir_exists(const char *path);
void path_append(char *lhs, const char *rhs);
void path_appendn(char *lhs, const char *rhs, u32 sz);
b8 mkpath(const char *path);
b8 app_data_dir(char *path, u32 n);
#ifdef VLT_USE_TINYDIR
b8 rmdir_f(const char *path);
#endif

#ifdef _WIN32
#include "violet/core/windows.h"
typedef HMODULE lib_handle;
#else
typedef void* lib_handle;
#endif

lib_handle lib_load(const char *filename);
void *lib_func(lib_handle hnd, const char *name);
b8 lib_close(lib_handle hnd);
const char *lib_err();

void exec(char *const argv[]);
int run(const char *command);

#endif
