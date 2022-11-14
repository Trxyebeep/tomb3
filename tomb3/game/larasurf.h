#pragma once
#include "../global/vars.h"

void inject_larasurf(bool replace);

void LaraSurface(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_surfswim(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_surfback(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_surfleft(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_surfright(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_surftread(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_surfswim(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_surfback(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_surfleft(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_surfright(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_surftread(ITEM_INFO* item, COLL_INFO* coll);

#define LaraTestWaterClimbOut	( (long(__cdecl*)(ITEM_INFO*, COLL_INFO*)) 0x0044E450 )
#define LaraSurfaceCollision	( (void(__cdecl*)(ITEM_INFO*, COLL_INFO*)) 0x0044E670 )
