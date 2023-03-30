#pragma once
#include "../global/types.h"

void LaraRapidsDrown();
void KayakInitialise(short item_number);
void KayakCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
long KayakControl();
void KayakDraw(ITEM_INFO* item);

enum KAYAK_STATES
{
	KS_BACK,
	KS_POSE,
	KS_LEFT,
	KS_RIGHT,
	KS_CLIMBIN,
	KS_DEATHIN,
	KS_FORWARD,
	KS_ROLL,
	KS_DROWNIN,
	KS_JUMPOUT,
	KS_TURNL,
	KS_TURNR,
	KS_CLIMBINR,
	KS_CLIMBOUTL,
	KS_CLIMBOUTR
};
