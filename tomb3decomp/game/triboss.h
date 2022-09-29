#pragma once
#include "../global/vars.h"

void inject_triboss(bool replace);

void InitialiseTribeBoss(short item_number);
void TriggerSummonSmoke(long x, long y, long z);
void TriggerLizardMan();

#define FindClosestShieldPoint	( (void(__cdecl*)(long, long, long, ITEM_INFO*)) 0x00471680 )
