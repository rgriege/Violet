#ifndef VIOLET_TIME_H
#define VIOLET_TIME_H

#include "violet/core/types.h"

#ifdef __unix__

#define __USE_POSIX199309
#include <time.h>

typedef struct timespec vlt_time;

#elif(_WIN32)

#include "violet/core/windows.h"

typedef LARGE_INTEGER vlt_time;

#endif

void vlt_get_time(vlt_time *t);
u32 vlt_diff_milli(const vlt_time *start, const vlt_time *end);
void vlt_sleep_milli(u32 milli);

#endif
