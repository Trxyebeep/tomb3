#pragma once
#include "../global/vars.h"

void inject_ds(bool replace);

bool DS_IsChannelPlaying(long num);
long DS_GetFreeChannel();

#define DS_IsSoundEnabled	( (bool(__cdecl*)()) 0x00480D40 )
#define DS_FreeAllSamples	( (void(__cdecl*)()) 0x00480600 )
#define DS_MakeSample	( (bool(__cdecl*)(long, LPWAVEFORMATEX, LPVOID, ulong)) 0x00480630 )
#define DS_Finish	( (void(__cdecl*)()) 0x00480D10 )
