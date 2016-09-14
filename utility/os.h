#ifndef VIOLET_OS_H
#define VIOLET_OS_H

#include "violet/core/types.h"

b8 vlt_file_open_dialog(char *filename, u32 n, const char *ext);
b8 vlt_file_save_dialog(char *filename, u32 n, const char *ext);

void path_append(char *lhs, const char *rhs);
b8 mkdir(const char *path);
b8 app_data_dir(char *path, u32 n);

#endif
