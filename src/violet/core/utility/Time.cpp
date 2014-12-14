#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

#include "violet/core/utility/Time.h"

using namespace Violet;

#ifdef _WIN32

long unsigned Time::getTimeInMilliseconds() {
	return timeGetTime();
}

double Time::getTimeInSeconds() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    return st.wHour * 3600.0 + st.wMinute * 60.0 + st.wSecond + st.wMilliseconds / 1000.0;
}

#else // Linux

long int Time::getTimeInMilliseconds() {
    timeval tim;
    gettimeofday(&tim, NULL);
    return tim.tv_sec * 1000 + tim.tv_usec/1000;
}

double Time::getTimeInSeconds() {
    timeval tim;
    gettimeofday(&tim, NULL);
    return tim.tv_sec + tim.tv_usec / 1000000.0;
}

#endif