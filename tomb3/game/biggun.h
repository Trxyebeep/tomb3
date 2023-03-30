#pragma once
#include "../global/types.h"

void BigGunInitialise(short item_number);
void BigGunCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void BigGunDraw(ITEM_INFO* item);
long BigGunControl(COLL_INFO* coll);

enum BIGGUN_STATE
{
	BGUNS_GETON,
	BGUNS_GETOFF,
	BGUNS_UPDOWN,
	BGUNS_RECOIL
};
