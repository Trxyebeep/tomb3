#include "../tomb3/pch.h"
#include "demo.h"

void GetDemoInput()
{
	if (democount < 3608)
		input = demoptr[democount];
	else
		input = -1;

	if (input != -1)
		democount++;
}

void inject_demo(bool replace)
{
	INJECT(0x00423970, GetDemoInput, replace);
}
