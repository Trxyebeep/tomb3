#pragma once
#include "../global/types.h"

void SubInitialise(short item_number);
void SubCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void SubDraw(ITEM_INFO* item);
long SubControl();
void SubEffects(short item_number);

extern BITE_INFO sub_bites[6];
extern SUB_WAKE_PTS SubWakePts[32][2];
extern uchar SubWakeShade;
extern uchar SubCurrentStartWake;
