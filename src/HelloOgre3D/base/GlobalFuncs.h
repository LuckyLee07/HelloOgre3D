#ifndef __GLOBAL_FUNCS__
#define __GLOBAL_FUNCS__

#include <time.h>

//tolua_begin

struct timezone {
	int tz_minuteswest; /* 分钟西移：格林尼治时间以西的分钟数 */
	int tz_dsttime;     /* 夏令时的类型 */
};

#if defined(WINDOWS_STORE_RT)
struct timeval {
	long tv_sec;  /* 秒 */
	long tv_usec; /* 微秒 */
};
#endif

int gettimeofday(struct timeval* tv, struct timezone* tz);

//tolua_end

#endif  // __GLOBAL_FUNCS__
