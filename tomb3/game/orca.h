#pragma once
#include "../global/types.h"

void OrcaControl(short item_number);

enum ORCA_STATES
{
	ORCA_SLOW,
	ORCA_FAST,
	ORCA_JUMP,
	ORCA_SPLASH,
	ORCA_SLOW_BUTT,
	ORCA_FAST_BUTT,
	ORCA_BREACH,
	ORCA_ROLL180
};
