#pragma once
#include "../global/vars.h"

void inject_lasers(bool replace);

long LaraOnLOS(GAME_VECTOR* s, GAME_VECTOR* t);
void UpdateLaserShades();
void LaserControl(short item_number);
