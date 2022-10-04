#include "../tomb3/pch.h"
#include "inventry.h"

long GetDebouncedInput(long in)
{
	static long db;
	long i;

	i = ~db;
	db = in;
	return i & in;
}

void inject_inventry(bool replace)
{
	INJECT(0x00436FA0, GetDebouncedInput, replace);
}
