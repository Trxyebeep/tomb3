#pragma once
#include "../global/types.h"

void InitialiseShiva(short item_number);
void ShivaControl(short item_number);
void DrawShiva(ITEM_INFO* item);

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
