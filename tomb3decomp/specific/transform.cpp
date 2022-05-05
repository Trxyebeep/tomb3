#include "../tomb3/pch.h"
#include "transform.h"

void InitZTable()
{
	float zv;
	long z;

	z = 0;

	for (int i = 0; i < 20480; i++)
	{
		zv = float(z << 14);
		z++;
		ZTable[i * 2] = f_persp / zv;
		ZTable[(i * 2) + 1] = f_oneopersp * f_persp / zv;
	}
}

void inject_transform(bool replace)
{
	INJECT(0x0048D6A0, InitZTable, replace);
}
