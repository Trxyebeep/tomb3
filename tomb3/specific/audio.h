#pragma once
#include "../global/types.h"

BOOL __stdcall ACMEnumCallBack(HACMDRIVERID hadid, DWORD_PTR dwInstance, DWORD fdwSupport);
void ACMCloseFile();
bool ACMOpenFile(const char* name);
void ACMEmulateCDStop();
void ACMEmulateCDPlay(long track, long mode);
void ThreadACMEmulateCDPlay(long track, long mode);
long ACMGetTrackLocation();
void ACMMute();
void ACMSetVolume(long volume);
long ACMHandleNotifications();
long ACMSetupNotifications();
bool ACMInit();
void ACMClose();

extern long acm_volume;
