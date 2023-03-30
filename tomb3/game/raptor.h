#pragma once
#include "../global/types.h"

void RaptorControl(short item_number);

enum RAPTOR_STATES
{
	RAPTOR_EMPTY,
	RAPTOR_STOP,
	RAPTOR_WALK,
	RAPTOR_RUN,
	RAPTOR_ATTACK1,
	RAPTOR_DEATH,
	RAPTOR_WARNING,
	RAPTOR_ATTACK2,
	RAPTOR_ATTACK3
};
