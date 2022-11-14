#include "../tomb3/pch.h"
#include "demo.h"
#include "gameflow.h"

void GetDemoInput()
{
	if (democount < 3608)
		input = demoptr[democount];
	else
		input = -1;

	if (input != -1)
		democount++;
}

long DoDemoSequence(long level)
{
	static long l;

	if (level < 0 && !gameflow.num_demos)
		return EXIT_TO_TITLE;

	if (level >= 0)
		l = level;
	else
	{
		if (l >= gameflow.num_demos)
			l = 0;

		level = GF_valid_demos[l];
		l++;
	}

	return GF_DoLevelSequence(level, 3);
}

void inject_demo(bool replace)
{
	INJECT(0x00423970, GetDemoInput, replace);
	INJECT(0x004236B0, DoDemoSequence, replace);
}
