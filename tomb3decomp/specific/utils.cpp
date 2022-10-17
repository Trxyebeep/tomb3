#include "../tomb3/pch.h"
#include "utils.h"

static double start_us;
static double start;
static double period;

double UT_GetAccurateTimer()
{
	LARGE_INTEGER counter;

	if (QueryPerformanceCounter(&counter))
		return ((double)counter.QuadPart - start_us) * period;
	else
		return double(timeGetTime() - start) / (double)CLOCKS_PER_SEC;
}

void UT_InitAccurateTimer()
{
	LARGE_INTEGER fq;

	if (QueryPerformanceFrequency(&fq))
	{
		period = 1.0 / (double)fq.QuadPart;
		start_us = 0;
		start_us = UT_GetAccurateTimer();
	}
	else
	{
		period = 0;
		start = timeGetTime();
	}
}

void inject_utils(bool replace)
{
	INJECT(0x0048E440, UT_GetAccurateTimer, replace);
	INJECT(0x0048E3E0, UT_InitAccurateTimer, replace);
}
