#pragma once
#include "../global/types.h"

void CrocControl(short item_number);

enum CROC_STATES
{
	CROC_EMPTY,
	CROC_SWIM,
	CROC_ATTACK,
	CROC_DEATH
};
