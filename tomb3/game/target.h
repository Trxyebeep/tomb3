#pragma once
#include "../global/vars.h"

void inject_target(bool replace);

void InitialiseTarget(short item_number);
void TargetControl(short item_number);

#define ResetTargets	( (void(__cdecl*)()) 0x0046AC00 )

enum TARGET_STATES
{
	TARGET_RISE,
	TARGET_HIT1,
	TARGET_HIT2,
	TARGET_HIT3
};
