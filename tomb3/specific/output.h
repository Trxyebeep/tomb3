#pragma once
#include "../global/types.h"

void S_PrintShadow(short size, short* box, ITEM_INFO* item);
void S_SetupAboveWater(long underwater);
void S_SetupBelowWater(long underwater);
void S_OutputPolyList();
void S_InsertBackPolygon(long xmin, long ymin, long xmax, long ymax, long col);
long S_GetObjectBounds(short* box);
void mCalcPoint(long x, long y, long z, long* result);
void ProjectPCoord(long x, long y, long z, long* result, long cx, long cy, long fov);
long S_DumpCine();
void S_InitialiseScreen(long type);
void ScreenPartialDump();
long S_DumpScreen();
void ScreenClear(bool a);
void S_ClearScreen();
void AnimateTextures(long n);
void S_AnimateTextures(long n);
void S_InitialisePolyList(bool clearBackBuffer);
long GetFixedScale(long unit);
long GetRenderScale(long unit);

extern long framedump;
extern long water_effect;
extern bool bBlueEffect;
