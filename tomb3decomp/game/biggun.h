#pragma once
#include "../global/vars.h"

void inject_biggun(bool replace);

void BigGunInitialise(short item_number);
void BigGunCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void BigGunDraw(ITEM_INFO* item);
long BigGunControl(COLL_INFO* coll);
