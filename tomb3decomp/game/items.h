#pragma once
#include "../global/vars.h"

#define CreateEffect	( (short(__cdecl*)(short)) 0x0043B130 )
#define KillItem	( (void(__cdecl*)(short)) 0x0043AA90 )
#define CreateItem	( (short(__cdecl*)()) 0x0043ABE0 )
#define InitialiseItem	( (void(__cdecl*)(short)) 0x0043AC30 )
#define AddActiveItem	( (void(__cdecl*)(short)) 0x0043AF60 )
