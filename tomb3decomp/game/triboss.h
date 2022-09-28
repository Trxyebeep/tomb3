#pragma once
#include "../global/vars.h"

void inject_triboss(bool replace);

short FindLizardManItemNumber(short room_number);
void InitialiseTribeBoss(short item_number);

#define FindClosestShieldPoint	( (void(__cdecl*)(long, long, long, ITEM_INFO*)) 0x00471680 )
