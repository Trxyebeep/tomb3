#pragma once
#include "../global/vars.h"

void inject_minecart(bool replace);

void MineCartInitialise(short item_number);
void MineCartCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
long MineCartControl();
