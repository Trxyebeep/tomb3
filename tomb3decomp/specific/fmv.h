#pragma once
#include "../global/vars.h"

void inject_fmv(bool replace);

long FMV_Play(char* name);
long FMV_PlayIntro(char* name1, char* name2);

#define WinPlayFMV	( (void(__cdecl*)(const char*, bool)) 0x00483170 )
#define WinStopFMV	( (void(__cdecl*)(bool)) 0x00483470 )

extern long fmv_playing;
