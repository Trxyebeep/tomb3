#include "../tomb3/pch.h"
#include "text.h"

short T_GetStringLen(char* string)
{
	short len;

	len = 1;

	while (*string++)
	{
		len++;

		if (len > 64)
			return 64;
	}

	return len;
}

long T_RemovePrint(TEXTSTRING* string)
{
	if (!string || !(string->flags & T_ACTIVE))
		return 0;

	string->flags &= ~T_ACTIVE;
	T_numStrings--;
	return 1;
}

void inject_text(bool replace)
{
	INJECT(0x0046B0C0, T_GetStringLen, replace);
	INJECT(0x0046B090, T_RemovePrint, replace);
}
