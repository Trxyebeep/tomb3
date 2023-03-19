#pragma once
#include "../global/vars.h"

void inject_croc(bool replace);

void CrocControl(short item_number);

enum CROC_STATES
{
	CROC_EMPTY,
	CROC_SWIM,
	CROC_ATTACK,
	CROC_DEATH
};
