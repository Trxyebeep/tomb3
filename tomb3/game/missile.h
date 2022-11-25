#pragma once
#include "../global/vars.h"

void inject_missile(bool replace);

void ControlMissile(short fx_number);

#define ExplodingDeath	( (void(__cdecl*)(short, long, short)) 0x00455370 )
#define ShootAtLara	( (void(__cdecl*)(FX_INFO*)) 0x004552C0 )
