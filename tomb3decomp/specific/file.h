#pragma once
#include "../global/vars.h"

void inject_file(bool replace);

bool LoadPalette(HANDLE file);

#define MyReadFile	( (void(__cdecl*)(HANDLE, void*, ulong, ulong*, OVERLAPPED*)) 0x00480D50 )
