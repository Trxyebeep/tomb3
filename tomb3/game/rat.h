#pragma once
#include "../global/vars.h"

void inject_rat(bool replace);

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
