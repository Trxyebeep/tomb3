#pragma once
#include "../global/types.h"

void MineCartInitialise(short item_number);
void MineCartCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
long MineCartControl();
