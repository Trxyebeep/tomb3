#pragma once
#include "../global/vars.h"

void inject_specific(bool replace);

uchar SWR_FindNearestPaletteEntry(uchar* p, long r, long g, long b, bool ignoreSystemPalette);
bool CD_Init();
long CalcVolume(long volume);
long CalcPan(short angle);
void S_SoundSetMasterVolume(long volume);
long S_SoundPlaySample(long num, ushort volume, long pitch, short pan);
long S_SoundPlaySampleLooped(long num, ushort volume, long pitch, short pan);
void S_SoundSetPanAndVolume(long num, short angle, ushort volume);
void S_SoundSetPitch(long num, long pitch);
void S_SoundStopSample(long num);
void S_SoundStopAllSamples();
long S_SoundSampleIsPlaying(long num);
void S_CDLoop();

#define S_CDStop	( (void(__cdecl*)()) 0x0048D4A0 )
#define S_CDPlay	( (void(__cdecl*)(short, long)) 0x0048D480 )
#define S_CDVolume	( (void(__cdecl*)(long)) 0x0048D4F0 )
#define S_StartSyncedAudio	( (long(__cdecl*)(long)) 0x0048D4B0 )
#define S_CDGetLoc	( (long(__cdecl*)()) 0x0048D4E0 )
