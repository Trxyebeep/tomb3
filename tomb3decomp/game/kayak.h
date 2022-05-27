#pragma once
#include "../global/vars.h"

void inject_kayak(bool replace);

void LaraRapidsDrown();
void KayakInitialise(short item_number);

#define KayakControl	( (long(__cdecl*)()) 0x0043B730 )
