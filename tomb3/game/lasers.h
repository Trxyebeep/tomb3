#pragma once
#include "../global/types.h"

long LaraOnLOS(GAME_VECTOR* s, GAME_VECTOR* t);
void UpdateLaserShades();
void LaserControl(short item_number);
void S_DrawLaser(ITEM_INFO* item);

extern uchar LaserShades[32];
