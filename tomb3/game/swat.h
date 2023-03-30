#pragma once
#include "../global/types.h"

void InitialiseSwat(short item_number);
void SwatControl(short item_number);

enum SWAT_STATES
{
	SWAT_EMPTY,
	SWAT_STOP,
	SWAT_WALK,
	SWAT_RUN,
	SWAT_WAIT,
	SWAT_SHOOT1,
	SWAT_SHOOT2,
	SWAT_DEATH,
	SWAT_AIM1,
	SWAT_AIM2,
	SWAT_AIM3,
	SWAT_SHOOT3
};
