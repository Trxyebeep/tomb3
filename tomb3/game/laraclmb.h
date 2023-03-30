#pragma once
#include "../global/types.h"

long LaraCheckForLetGo(ITEM_INFO* item, COLL_INFO* coll);
void LaraDoClimbLeftRight(ITEM_INFO* item, COLL_INFO* coll, long result, long shift);
void lara_as_climbleft(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbright(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbstnc(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbing(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbend(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbdown(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbleft(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbright(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbstnc(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbing(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbdown(ITEM_INFO* item, COLL_INFO* coll);
long LaraTestClimb(long x, long y, long z, long xfront, long zfront, long item_height, short item_room, long* shift);
long LaraTestClimbPos(ITEM_INFO* item, long front, long right, long origin, long height, long* shift);
long LaraTestClimbUpPos(ITEM_INFO* item, long front, long right, long* shift, long* ledge);
