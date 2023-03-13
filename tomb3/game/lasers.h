#pragma once
#include "../global/vars.h"

void inject_lasers(bool replace);

#define LaraOnLOS	( (long(__cdecl*)(GAME_VECTOR*, GAME_VECTOR*)) 0x0044F580 )
#define UpdateLaserShades	( (void(__cdecl*)()) 0x0044F7A0 )
