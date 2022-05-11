#pragma once
#include "../global/vars.h"

#define WadeSplash	( (void(__cdecl*)(ITEM_INFO*, long, long)) 0x0042E9F0 )
#define Splash	( (void(__cdecl*)(ITEM_INFO*)) 0x0042E8C0 )
#define DoBloodSplat	( (void(__cdecl*)(long, long, long, short, short, short)) 0x0042E2C0 )
