#pragma once
#include "../global/vars.h"

void phd_PopMatrix_I();
void phd_PushMatrix_I();
void phd_RotY_I(short angle);
void phd_RotX_I(short angle);
void phd_RotZ_I(short angle);
void phd_TranslateRel_I(long x, long y, long z);
void phd_TranslateRel_ID(long x, long y, long z, long x2, long y2, long z2);
void phd_RotYXZ_I(short y, short x, short z);
void phd_PutPolygons_I(short* objptr, long clip);
void gar_RotYXZsuperpack(short** pprot, long skip);
void gar_RotYXZsuperpack_I(short** pprot1, short** pprot2, long skip);
void InitInterpolate(long frac, long rate);
void InterpolateMatrix();
void InterpolateArmMatrix();
long GetFrames(ITEM_INFO* item, short* frm[], long* rate);
short* GetBestFrame(ITEM_INFO* item);
short* GetBoundsAccurate(ITEM_INFO* item);
void SetRoomBounds(short* door, long rn, ROOM_INFO* actualRoom);
