#pragma once
#include "../global/vars.h"

#define CreateEffect	( (short(__cdecl*)(short)) 0x0043B130 )
#define KillItem	( (void(__cdecl*)(short)) 0x0043AA90 )
#define KillEffect	( (void(__cdecl*)(short)) 0x0043B1A0 )
#define CreateItem	( (short(__cdecl*)()) 0x0043ABE0 )
#define InitialiseItem	( (void(__cdecl*)(short)) 0x0043AC30 )
#define InitialiseItemArray	( (void(__cdecl*)(short)) 0x0043AA20 )
#define AddActiveItem	( (void(__cdecl*)(short)) 0x0043AF60 )
#define ItemNewRoom	( (void(__cdecl*)(short, short)) 0x0043AFD0 )
#define EffectNewRoom	( (void(__cdecl*)(short, short)) 0x0043B290 )
#define RemoveActiveItem	( (void(__cdecl*)(short)) 0x0043AE40 )
#define RemoveDrawnItem	( (void(__cdecl*)(short)) 0x0043AEE0 )
#define GlobalItemReplace	( (void(__cdecl*)(long, long)) 0x0043B080 )
