#pragma once
#include "../global/types.h"

void InitialiseTarget(short item_number);
void TargetControl(short item_number);
void ResetTargets();

enum TARGET_STATES
{
	TARGET_RISE,
	TARGET_HIT1,
	TARGET_HIT2,
	TARGET_HIT3
};
