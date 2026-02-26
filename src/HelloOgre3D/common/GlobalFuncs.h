#ifndef __GLOBAL_FUNCS__
#define __GLOBAL_FUNCS__

#include <time.h>
#if !defined(_WIN32)
#include <sys/time.h>
#endif

//tolua_begin

#if defined(_WIN32)
struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};

int gettimeofday(struct timeval* tv, struct timezone* tz);
#endif

//tolua_end

template <typename T>
T clamp(T min, T max, T value) {
    if (value < min) {
        return min;
    }
    else if (value > max) {
        return max;
    }
    return value;
}

#endif  // __GLOBAL_FUNCS__
