#pragma once
#include "../global/vars.h"

void inject_compy(bool replace);

void InitialiseCompy(short item_number);
void CompyControl(short item_number);
void CarcassControl(short item_number);

enum COMPY_STATES
{
	COMPY_STOP,
	COMPY_RUN,
	COMPY_JUMP,
	COMPY_ATTACK,
	COMPY_DEATH
};
