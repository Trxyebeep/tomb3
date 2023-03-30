#pragma once
#include "../global/types.h"

void ControlClawmutePlasmaBall(short fx_number);
void ClawmuteControl(short item_number);

enum CLAWMUTE_STATES
{
	CLAW_STOP,
	CLAW_WALK,
	CLAW_RUN,
	CLAW_RUN_ATAK,
	CLAW_WALK_ATAK1,
	CLAW_WALK_ATAK2,
	CLAW_SLASH_LEFT,
	CLAW_SLASH_RIGHT,
	CLAW_DEATH,
	CLAW_CLAW_ATAK,
	CLAW_FIRE_ATAK
};
