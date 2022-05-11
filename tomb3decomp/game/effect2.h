#pragma once
#include "../global/vars.h"

#define TriggerWaterfallMist	( (void(__cdecl*)(long, long, long, long)) 0x0042D1F0 )
#define TriggerGunSmoke	( (void(__cdecl*)(long, long, long, long, long, long, long, long, long)) 0x0042B4F0 )
#define TriggerGunShell	( (void(__cdecl*)(short, long, long)) 0x0042BE50 )
#define TriggerExplosionSparks	( (void(__cdecl*)(long, long, long, long, long, long, short)) 0x0042AB80 )
#define TriggerExplosionSmoke	( (void(__cdecl*)(long, long, long, long)) 0x0042B130 )
