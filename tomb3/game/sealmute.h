#pragma once
#include "../global/types.h"

void SealmuteControl(short item_number);

enum SEAL_STATES
{
	SEAL_STOP,
	SEAL_WALK,
	SEAL_BURP,
	SEAL_DEATH
};
