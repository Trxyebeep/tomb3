#include "../tomb3/pch.h"
#include "phd_math.h"

ulong __fastcall phd_sqrt(ulong num)
{
	ulong base, result, tmp;

	base = 0x40000000;
	result = 0;

	do
	{
		tmp = result;
		result += base;
		tmp >>= 1;

		if (result > num)
			result = tmp;
		else
		{
			num -= result;
			result = base | tmp;
		}

		base >>= 2;

	} while (base);

	return result;
}

void inject_phdmath(bool replace)
{
	INJECT(0x004B4C93, phd_sqrt, replace);
}
