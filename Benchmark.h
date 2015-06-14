/*
 * Benchmark.h
 *
 *  Created on: 14/06/2015
 *      Author: lucas
 */

#ifndef BENCHMARK_H_
#define BENCHMARK_H_
#ifdef WIN32

#include <windows.h>
double get_time()
{
    LARGE_INTEGER t, f;
    QueryPerformanceCounter(&t);
    QueryPerformanceFrequency(&f);
    return (double)t.QuadPart/(double)f.QuadPart;
}

#else

#include <time.h>

float getTime()
{
    return (float)clock()/CLOCKS_PER_SEC;
}

#endif


#endif /* BENCHMARK_H_ */
