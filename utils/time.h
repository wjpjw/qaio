#pragma once
#include <stdint.h>

namespace wjp{

using time = uint64_t;  //ms 

timespec get_ts(time ms){
    struct timespec ts;
	ts.tv_sec  = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
    return ts;
}

timespec get_ts_after(time ms){
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += ms/1000;
    ts.tv_nsec += (ms % 1000) * 1000000;
}

time now()
{
	struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
	return 1000*ts.tv_sec+(ts.tv_nsec / 1000000);
}

void sleep(time ms)
{
	struct timespec ts;
	ts.tv_sec  = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	while (ts.tv_sec || ts.tv_nsec) {
		if (nanosleep(&ts, &ts) == 0) {
			break;
		}
	}
}

}