#pragma once
#include "../global/vars.h"

void inject_specific(bool replace);

uchar SWR_FindNearestPaletteEntry(uchar* p, long r, long g, long b, bool ignoreSystemPalette);
bool CD_Init();
long CalcVolume(long volume);
void S_SoundSetMasterVolume(long volume);

#define S_CDStop	( (void(__cdecl*)()) 0x0048D4A0 )
#define S_CDPlay	( (void(__cdecl*)(short, long)) 0x0048D480 )
#define S_CDVolume	( (void(__cdecl*)(long)) 0x0048D4F0 )
#define S_CDLoop	( (void(__cdecl*)()) 0x0048D260 )
#define S_StartSyncedAudio	( (long(__cdecl*)(long)) 0x0048D4B0 )
#define S_CDGetLoc	( (long(__cdecl*)()) 0x0048D4E0 )
#define S_SoundStopAllSamples	( (void(__cdecl*)()) 0x0048D230 )
