#pragma once
#include "../global/vars.h"

void inject_sealmute(bool replace);

void SealmuteControl(short item_number);

enum SEAL_STATES
{
	SEAL_STOP,
	SEAL_WALK,
	SEAL_BURP,
	SEAL_DEATH
};
