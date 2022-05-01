#pragma once
#include "../global/vars.h"

void inject_lara(bool replace);

long LaraLandedBad(ITEM_INFO* item, COLL_INFO* coll);
long TestLaraSlide(ITEM_INFO* item, COLL_INFO* coll);
long LaraTestClimbStance(ITEM_INFO* item, COLL_INFO* coll);
long TestLaraVault(ITEM_INFO* item, COLL_INFO* coll);
void lara_slide_slope(ITEM_INFO* item, COLL_INFO* coll);
void GetLaraCollisionInfo(ITEM_INFO* item, COLL_INFO* coll);
long LaraHitCeiling(ITEM_INFO* item, COLL_INFO* coll);
long LaraDeflectEdge(ITEM_INFO* item, COLL_INFO* coll);
void LaraDeflectEdgeJump(ITEM_INFO* item, COLL_INFO* coll);
void LaraSlideEdgeJump(ITEM_INFO* item, COLL_INFO* coll);
long TestHangSwingIn(ITEM_INFO* item, short angle);
long LaraFallen(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_duck(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_duck(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_all4s(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_all4s(ITEM_INFO* item, COLL_INFO* coll);
short LaraFloorFront(ITEM_INFO* item, short ang, long dist);
short LaraCeilingFront(ITEM_INFO* item, short ang, long dist);

#define LookUpDown	( (void(__cdecl*)()) 0x004446E0 )
#define GetStaticObjects	( (long(__cdecl*)(ITEM_INFO*, short, long, long, long)) 0x00445020 )
