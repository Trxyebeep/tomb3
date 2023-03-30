#pragma once
#include "../global/types.h"

bool DS_IsChannelPlaying(long num);
long DS_GetFreeChannel();
long DS_StartSample(long num, long volume, long pitch, long pan, ulong flags);
void DS_FreeAllSamples();
bool DS_MakeSample(long num, LPWAVEFORMATEX fmt, LPVOID data, ulong bytes);
void DS_AdjustVolumeAndPan(long num, long volume, long pan);
void DS_AdjustPitch(long num, long pitch);
void DS_StopSample(long num);
bool DS_Create(LPGUID lpGuid);
bool DS_IsSoundEnabled();
bool DS_SetOutputFormat();
void DS_Start(HWND hwnd);
void DS_Finish();
BOOL CALLBACK DS_EnumCallback(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext);
bool DS_EnumerateDevices(LPVOID lpContext);
bool DS_MakeAdapterList();
void DS_Init();

extern LPDIRECTSOUNDBUFFER DS_Samples[32];
extern LPDIRECTSOUND lpDirectSound;
