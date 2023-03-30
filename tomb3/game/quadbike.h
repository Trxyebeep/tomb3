#pragma once
#include "../global/types.h"

void QuadBikeDraw(ITEM_INFO* item);
void InitialiseQuadBike(short item_number);
void QuadBikeCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
long QuadBikeControl();

enum QUAD_STATES
{
	QS_0,
	QS_DRIVE,
	QS_TURNL,
	QS_3,
	QS_4,
	QS_SLOW,
	QS_BRAKE,
	QS_BIKEDEATH,
	QS_FALL,
	QS_GETONR,
	QS_GETOFFR,
	QS_HITBACK,
	QS_HITFRONT,
	QS_HITLEFT,
	QS_HITRIGHT,
	QS_STOP,
	QS_16,
	QS_LAND,
	QS_STOPSLOWLY,
	QS_FALLDEATH,
	QS_FALLOFF,
	QS_WHEELIE,
	QS_TURNR,
	QS_GETONL,
	QS_GETOFFL
};
