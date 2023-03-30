#pragma once
#include "../global/types.h"

void MouseControl(short item_number);

enum RAT_STATES
{
	MOUSE_EMPTY,
	MOUSE_RUN,
	MOUSE_STOP,
	MOUSE_WAIT1,
	MOUSE_WAIT2,
	MOUSE_ATTACK,
	MOUSE_DEATH
};
