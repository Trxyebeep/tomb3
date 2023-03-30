#pragma once
#include "../global/types.h"

void LaraUnderWater(ITEM_INFO* item, COLL_INFO* coll);
void SwimTurn(ITEM_INFO* item);
void lara_as_swim(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_glide(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_tread(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_dive(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_uwdeath(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_waterroll(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_swim(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_glide(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_tread(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_dive(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_uwdeath(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_waterroll(ITEM_INFO* item, COLL_INFO* coll);
long GetWaterDepth(long x, long y, long z, short room_number);
void LaraTestWaterDepth(ITEM_INFO* item, COLL_INFO* coll);
void LaraWaterCurrent(COLL_INFO* coll);
void LaraSwimCollision(ITEM_INFO* item, COLL_INFO* coll);
