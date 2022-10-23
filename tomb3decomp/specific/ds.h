#pragma once
#include "../global/vars.h"

void inject_ds(bool replace);

bool DS_IsChannelPlaying(long num);
long DS_GetFreeChannel();
long DS_StartSample(long num, long volume, long pitch, long pan, ulong flags);
void DS_FreeAllSamples();

#define DS_IsSoundEnabled	( (bool(__cdecl*)()) 0x00480D40 )
#define DS_MakeSample	( (bool(__cdecl*)(long, LPWAVEFORMATEX, LPVOID, ulong)) 0x00480630 )
#define DS_Finish	( (void(__cdecl*)()) 0x00480D10 )
