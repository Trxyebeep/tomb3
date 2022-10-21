#pragma once
#include "../global/vars.h"

void inject_dx(bool replace);

void DX_SaveScreen(LPDIRECTDRAWSURFACE3 surf);
void DX_DoFlipWait();

#define DX_ClearBuffers	( (void(__cdecl*)(ulong, ulong)) 0x004B3A70 )
#define DX_UpdateFrame	( (void(__cdecl*)(bool, RECT*)) 0x004B3D10 )
