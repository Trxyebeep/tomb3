#pragma once
#include "../global/types.h"

void OldWinstonControl(short item_number);
void WinstonControl(short item_number);

enum WINSTON_STATES
{
	WINSTON_EMPTY,
	WINSTON_STOP,
	WINSTON_WALK,
	WINSTON_DEF1,
	WINSTON_DEF2,
	WINSTON_DEF3,
	WINSTON_HIT1,
	WINSTON_HIT2,
	WINSTON_HIT3,
	WINSTON_HITDOWN,
	WINSTON_FALLDOWN,
	WINSTON_GETUP,
	WINSTON_BRUSHOFF,
	WINSTON_ONFLOOR
};
