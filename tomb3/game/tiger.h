#pragma once
#include "../global/types.h"

void TigerControl(short item_number);

enum TIGER_STATES
{
	TIGER_EMPTY,
	TIGER_STOP,
	TIGER_WALK,
	TIGER_RUN,
	TIGER_WAIT,
	TIGER_ROAR,
	TIGER_ATTACK1,
	TIGER_ATTACK2,
	TIGER_ATTACK3,
	TIGER_DEATH
};
