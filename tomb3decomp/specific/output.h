#pragma once
#include "../global/vars.h"

void inject_output(bool replace);

void S_PrintShadow(short size, short* box, ITEM_INFO* item);
void S_SetupAboveWater(long underwater);
void S_SetupBelowWater(long underwater);
void S_OutputPolyList();
void S_LightRoom(ROOM_INFO* r);
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

#ifdef TROYESTUFF
long GetRenderScale(long unit);
#endif

#define S_InitialisePolyList	( (void(__cdecl*)(long)) 0x0048A210 )
#define S_AnimateTextures	( (void(__cdecl*)(long)) 0x0048AB50 )
