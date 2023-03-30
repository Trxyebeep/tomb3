#pragma once
#include "../global/types.h"

void ControlElectricFence(short item_number);
void InitialiseCivvy(short item_number);
void CivvyControl(short item_number);

enum CIVVY_STATES
{
	CIVVY_EMPTY,
	CIVVY_STOP,
	CIVVY_WALK,
	CIVVY_PUNCH2,
	CIVVY_AIM2,
	CIVVY_WAIT,
	CIVVY_AIM1,
	CIVVY_AIM0,
	CIVVY_PUNCH1,
	CIVVY_PUNCH0,
	CIVVY_RUN,
	CIVVY_DEATH,
	CIVVY_CLIMB3,
	CIVVY_CLIMB1,
	CIVVY_CLIMB2,
	CIVVY_FALL3
};
