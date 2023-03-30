#pragma once
#include "../global/types.h"

void InitialiseArmySMG(short item_number);
void ArmySMGControl(short item_number);

enum ARMYSMG_STATES
{
	ARMY_EMPTY,
	ARMY_STOP,
	ARMY_WALK,
	ARMY_RUN,
	ARMY_WAIT,
	ARMY_SHOOT1,
	ARMY_SHOOT2,
	ARMY_DEATH,
	ARMY_AIM1,
	ARMY_AIM2,
	ARMY_AIM3,
	ARMY_SHOOT3
};
