#pragma once
#include "../global/vars.h"

void inject_triboss(bool replace);

void InitialiseTribeBoss(short item_number);
void TriggerSummonSmoke(long x, long y, long z);
void TriggerLizardMan();
void TriggerElectricSparks(GAME_VECTOR* pos, long shield);
void FindClosestShieldPoint(long x, long y, long z, ITEM_INFO* item);
