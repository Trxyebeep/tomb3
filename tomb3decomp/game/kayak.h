#pragma once
#include "../global/vars.h"

void inject_kayak(bool replace);

void LaraRapidsDrown();
void KayakInitialise(short item_number);
void KayakCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
long KayakControl();
