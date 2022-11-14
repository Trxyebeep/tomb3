#pragma once
#include "../global/vars.h"

void inject_triboss(bool replace);

void InitialiseTribeBoss(short item_number);
void TriggerSummonSmoke(long x, long y, long z);
void TriggerLizardMan();
void TriggerElectricSparks(GAME_VECTOR* pos, long shield);
void FindClosestShieldPoint(long x, long y, long z, ITEM_INFO* item);
void S_DrawTribeBoss(ITEM_INFO* item);
void TribeBossControl(short item_number);

extern BITE_INFO tribeboss_hit[6];
extern SHIELD_POINTS TribeBossShield[40];
extern PHD_VECTOR TrigDynamics[3];
extern char shield_active;
