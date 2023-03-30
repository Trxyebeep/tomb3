#pragma once
#include "../global/types.h"

void InitialiseWingmute(short item_number);
void WingmuteControl(short item_number);

enum WINGMUTE_STATES
{
	WING_HOVER,
	WING_LAND,
	WING_WAIT,
	WING_TAKEOFF,
	WING_ATTACK,
	WING_FALL,
	WING_DEATH,
	WING_MOVE
};
