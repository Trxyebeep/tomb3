#pragma once
#include "../global/types.h"

void InitialiseBoat(short item_number);
void BoatCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void DrawBoat(ITEM_INFO* item);
void BoatControl(short item_number);

enum BOAT_STATES
{
	BOAT_GETON,
	BOAT_STILL,
	BOAT_MOVING,
	BOAT_JUMPR,
	BOAT_JUMPL,
	BOAT_HIT,
	BOAT_FALL,
	BOAT_TURNR,
	BOAT_DEATH,
	BOAT_TURNL
};
