#pragma once
#include "../global/vars.h"

void inject_input(bool replace);

long Key(long number);

#define S_UpdateInput	( (long(__cdecl*)()) 0x00486260 )
