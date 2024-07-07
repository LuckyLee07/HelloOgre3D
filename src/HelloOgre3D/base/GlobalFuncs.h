#ifndef __GLOBAL_FUNCS__
#define __GLOBAL_FUNCS__

#include <time.h>

//tolua_begin

struct timezone {
	int tz_minuteswest; /* �������ƣ���������ʱ�������ķ����� */
	int tz_dsttime;     /* ����ʱ������ */
};

#if defined(WINDOWS_STORE_RT)
struct timeval {
	long tv_sec;  /* �� */
	long tv_usec; /* ΢�� */
};
#endif

int gettimeofday(struct timeval* tv, struct timezone* tz);

//tolua_end

#endif  // __GLOBAL_FUNCS__
