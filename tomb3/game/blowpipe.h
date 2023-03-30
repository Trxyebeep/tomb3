#pragma once
#include "../global/types.h"

void BlowpipeControl(short item_number);

enum BLOW_STATES
{
	BLOW_EMPTY,
	BLOW_WAIT1,
	BLOW_WALK,
	BLOW_RUN,
	BLOW_ATTACK1,
	BLOW_ATTACK2,
	BLOW_ATTACK3,
	BLOW_ATTACK4,
	BLOW_AIM3,
	BLOW_DEATH,
	BLOW_ATTACK5,
	BLOW_WAIT2
};
