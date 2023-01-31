#pragma once
#include "../global/vars.h"

void inject_laraclmb(bool replace);

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

#define LaraTestClimbPos	( (long(__cdecl*)(ITEM_INFO*, long, long, long, long, long*)) 0x00449090 )
#define LaraTestClimbUpPos	( (long(__cdecl*)(ITEM_INFO*, long, long, long*, long*)) 0x00448E60 )
