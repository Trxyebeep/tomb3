#pragma once
#include "../global/vars.h"

void inject_effect2(bool replace);

void TriggerDynamic(long x, long y, long z, long falloff, long r, long g, long b);
void ClearDynamics();
void KillEverything();
void TriggerBreath(long x, long y, long z, long xv, long yv, long zv);

#define TriggerWaterfallMist	( (void(__cdecl*)(long, long, long, long)) 0x0042D1F0 )
#define TriggerGunSmoke	( (void(__cdecl*)(long, long, long, long, long, long, long, long, long)) 0x0042B4F0 )
#define TriggerGunShell	( (void(__cdecl*)(short, long, long)) 0x0042BE50 )
#define TriggerExplosionSparks	( (void(__cdecl*)(long, long, long, long, long, long, short)) 0x0042AB80 )
#define TriggerExplosionSmoke	( (void(__cdecl*)(long, long, long, long)) 0x0042B130 )
#define GetFreeSpark	( (long(__cdecl*)()) 0x00429FE0 )
#define TriggerFlareSparks	( (void(__cdecl*)(long, long, long, long, long, long, long, long)) 0x0042A8B0 )
#define CreateBubble	( (void(__cdecl*)(PHD_3DPOS*, short, long, long)) 0x0042E4F0 )
#define TriggerRocketFlame	( (void(__cdecl*)(long, long, long, long, long, long, long)) 0x0042C510 )
