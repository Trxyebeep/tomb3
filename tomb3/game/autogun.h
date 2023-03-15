#pragma once
#include "../global/vars.h"

void inject_autogun(bool replace);

void InitialiseAutogun(short item_number);

enum AUTOGUN_STATES
{
	AUTOGUN_FIRE,
	AUTOGUN_STILL
};
