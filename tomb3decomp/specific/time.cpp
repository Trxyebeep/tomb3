#include "../tomb3/pch.h"
#include "time.h"

static __int64 counter, frequency;

void TIME_Reset()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&counter);
}

bool TIME_Init()
{
	__int64 pfq;

	if (!QueryPerformanceFrequency((LARGE_INTEGER*)&pfq))
		return 0;

	frequency = pfq / 60;
	TIME_Reset();
	return 1;
}

long Sync()
{
	__int64 PerformanceCount, f;
	long n;

	QueryPerformanceCounter((LARGE_INTEGER*)&PerformanceCount);
	f = (PerformanceCount - counter) / frequency;
	counter += frequency * f;
	n = (long)f;
	return n;
}

void inject_time(bool replace)
{
	INJECT(0x0048D5D0, TIME_Reset, replace);
	INJECT(0x0048D5E0, TIME_Init, replace);
	INJECT(0x0048D630, Sync, replace);
}
