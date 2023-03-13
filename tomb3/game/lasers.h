#pragma once
#include "../global/vars.h"

void inject_lasers(bool replace);

long LaraOnLOS(GAME_VECTOR* s, GAME_VECTOR* t);

#define UpdateLaserShades	( (void(__cdecl*)()) 0x0044F7A0 )
