#include "../tomb3/pch.h"
#include "time.h"

static __int64 frequency, ticks;

static void UpdateTicks()
{
	__int64 counter;

	QueryPerformanceCounter((LARGE_INTEGER*)&counter);
	ticks = counter;
}

bool TIME_Init()
{
	__int64 pfq;

	if (!QueryPerformanceFrequency((LARGE_INTEGER*)&pfq))
		return 0;

	frequency = pfq / TICKS_PER_SECOND;
	UpdateTicks();
	return 1;
}

ulong Sync()
{
	__int64 last;

	last = ticks;
	UpdateTicks();
	return ulong(double(ticks - last) / frequency);
}

ulong SyncTicks(long skip)
{
	double passed, dskip;
	__int64 last;

	passed = 0;
	dskip = (double)skip;
	last = ticks;

	do
	{
		UpdateTicks();
		passed = double(ticks - last) / frequency;
	}
	while (passed < dskip);

	return (ulong)passed;
}
