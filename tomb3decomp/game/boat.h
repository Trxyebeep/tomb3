#pragma once
#include "../global/vars.h"

void InitialiseBoat(short item_number);
void BoatCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);

void inject_boat(bool replace);
