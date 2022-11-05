#pragma once
#include "../global/vars.h"

void inject_quadbike(bool replace);

void QuadBikeDraw(ITEM_INFO* item);
void InitialiseQuadBike(short item_number);
void QuadBikeCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
long QuadBikeControl();
