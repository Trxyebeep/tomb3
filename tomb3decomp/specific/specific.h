#pragma once
#include "../global/vars.h"

#define S_CDStop	( (void(__cdecl*)()) 0x0048D4A0 )
#define S_CDPlay	( (void(__cdecl*)(short, long)) 0x0048D480 )
#define S_CDVolume	( (void(__cdecl*)(long)) 0x0048D4F0 )
