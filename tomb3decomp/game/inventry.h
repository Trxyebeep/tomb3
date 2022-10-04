#pragma once
#include "../global/vars.h"

void inject_inventry(bool replace);

long GetDebouncedInput(long in);

#define Display_Inventory	( (long(__cdecl*)(long)) 0x00435400 )
