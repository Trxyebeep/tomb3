#include "../tomb3/pch.h"
#include "game.h"

static long rand_1 = 0xD371F947;
static long rand_2 = 0xD371F947;

long GetRandomControl()
{
	rand_1 = 0x41C64E6D * rand_1 + 0x3039;
	return (rand_1 >> 10) & 0x7FFF;
}

void SeedRandomControl(long seed)
{
	rand_1 = seed;
}

long GetRandomDraw()
{
	rand_2 = 0x41C64E6D * rand_2 + 0x3039;
	return (rand_2 >> 10) & 0x7FFF;
}

void SeedRandomDraw(long seed)
{
	rand_2 = seed;
}

void inject_sgame(bool replace)
{
	INJECT(0x004841F0, GetRandomControl, replace);
	INJECT(0x00484210, SeedRandomControl, replace);
	INJECT(0x00484220, GetRandomDraw, replace);
	INJECT(0x00484240, SeedRandomDraw, replace);
}
