#pragma once
#include "../global/vars.h"

#define S_ExitSystem	( (void(__cdecl*)(const char*)) 0x004B37C0 )
#define SaveDDBuffer	( (void(__cdecl*)(LPDIRECTDRAWSURFACE3)) 0x004B40A0 )
#define DD_SpinMessageLoop	( (bool(__cdecl*)(bool)) 0x004B3830 )