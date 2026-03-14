#include "GlobalFuncs.h"

#if defined(_WIN32)
#include <windows.h>

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS 11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS 11644473600000000ULL
#endif

int gettimeofday(struct timeval* tv, struct timezone* tz)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;

	static int tzflag;

	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		tmpres /= 10;
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	if (NULL != tz)
	{
		if (!tzflag)
		{
			_tzset();
			tzflag++;
		}

		long timezoneSeconds = 0;
		int daylight = 0;
		if (_get_timezone(&timezoneSeconds) == 0)
			tz->tz_minuteswest = static_cast<int>(timezoneSeconds / 60);
		else
			tz->tz_minuteswest = 0;

		if (_get_daylight(&daylight) == 0)
			tz->tz_dsttime = daylight;
		else
			tz->tz_dsttime = 0;
	}
	return 0;
}
#endif
