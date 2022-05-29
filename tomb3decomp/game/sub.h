#pragma once
#include "../global/vars.h"

void inject_sub(bool replace);

void SubInitialise(short item_number);
void SubCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void SubDraw(ITEM_INFO* item);
long SubControl();
void SubEffects(short item_number);
