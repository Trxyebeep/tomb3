#pragma once
#include "../global/vars.h"

#define WinDXInit	( (bool(__cdecl*)(DEVICEINFO*, DXCONFIG*, bool)) 0x004B2F80 )
#define WinFreeDX	( (void(__cdecl*)(bool)) 0x004B2C60 )
