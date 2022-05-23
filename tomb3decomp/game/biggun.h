#pragma once
#include "../global/vars.h"

void inject_biggun(bool replace);

void BigGunInitialise(short item_number);
void BigGunCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);

#define BigGunControl	( (long(__cdecl*)(COLL_INFO*)) 0x00411100 )
