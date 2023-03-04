#pragma once
#include "../global/vars.h"

void inject_shiva(bool replace);

void InitialiseShiva(short item_number);
void ShivaControl(short item_number);

enum SHIVA_STATES
{
	SHIVA_WAIT,
	SHIVA_WALK,
	SHIVA_WAIT_DEF,
	SHIVA_WALK_DEF,
	SHIVA_START,
	SHIVA_PINCER,
	SHIVA_KILL,
	SHIVA_CHOPPER,
	SHIVA_WALKBACK,
	SHIVA_DEATH
};
