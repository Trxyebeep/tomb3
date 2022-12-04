#pragma once
#include "../global/vars.h"

void inject_lot(bool replace);

void InitialiseLOTarray();

#define DisableBaddieAI	( (void(__cdecl*)(short)) 0x00452F90 )
#define EnableBaddieAI	( (long(__cdecl*)(short, long)) 0x00453000 )
