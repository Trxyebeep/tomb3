#pragma once
#include "../global/vars.h"

void inject_missile(bool replace);

void ControlMissile(short fx_number);
void ShootAtLara(FX_INFO* fx);

#define ExplodingDeath	( (void(__cdecl*)(short, long, short)) 0x00455370 )
