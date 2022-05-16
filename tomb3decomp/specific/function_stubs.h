#pragma once
#include "../global/vars.h"

#define GetRandomControl	( (long(__cdecl*)()) 0x004841F0 )
#define GetRandomDraw	( (long(__cdecl*)()) 0x00484220 )
#define mGetAngle	( (ulong(__cdecl*)(long, long, long, long)) 0x00483860)
