#pragma once
#include "../global/vars.h"

void inject_ds(bool replace);

bool DS_IsChannelPlaying(long num);
long DS_GetFreeChannel();
long DS_StartSample(long num, long volume, long pitch, long pan, ulong flags);
void DS_FreeAllSamples();
bool DS_MakeSample(long num, LPWAVEFORMATEX fmt, LPVOID data, ulong bytes);
void DS_AdjustVolumeAndPan(long num, long volume, long pan);
void DS_AdjustPitch(long num, long pitch);

#define DS_IsSoundEnabled	( (bool(__cdecl*)()) 0x00480D40 )
#define DS_Finish	( (void(__cdecl*)()) 0x00480D10 )
