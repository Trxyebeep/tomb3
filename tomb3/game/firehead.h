#pragma once
#include "../global/types.h"

void InitialiseFireHead(short item_number);
void ControlFireHead(short item_number);
void ControlRotateyThing(short item_number);

enum FIREHEAD_STATES
{
	FIREHEAD_STILL,
	FIREHEAD_REAR,
	FIREHEAD_BLOW
};

enum FIREHEAD_FLAGS
{
	FH_BLOWLOOPS,
	FH_SPEED,
	FH_STOP,
	FH_DEADLYRANGE
};
